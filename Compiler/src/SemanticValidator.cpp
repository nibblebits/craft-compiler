/*
    Craft Compiler v0.1.0 - The standard compiler for the Craft programming language.
    Copyright (C) 2016  Daniel McCarthy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   SemanticValidator.cpp
 * Author: Daniel McCarthy
 *
 * Created on 10 December 2016, 00:07
 * 
 * Description: Semantically validates the abstract syntax tree.
 */

#include "SemanticValidator.h"
#include "Tree.h"
#include "branches.h"

SemanticValidator::SemanticValidator(Compiler* compiler) : CompilerEntity(compiler)
{
    this->logger = std::shared_ptr<Logger>(new Logger());
}

SemanticValidator::~SemanticValidator()
{
}

void SemanticValidator::setTree(std::shared_ptr<Tree> tree)
{
    this->tree = tree;
}

void SemanticValidator::validate()
{
    // Validate the top of the tree
    validate_top(this->tree->root);
}

void SemanticValidator::validate_top(std::shared_ptr<RootBranch> root_branch)
{
    for (std::shared_ptr<Branch> child : root_branch->getChildren())
    {
        validate_part(child);
    }
}

void SemanticValidator::validate_part(std::shared_ptr<Branch> branch)
{
    std::string type = branch->getType();
    if (type == "FUNC")
    {
        validate_function(std::dynamic_pointer_cast<FuncBranch>(branch));
    }
    else if (type == "BODY")
    {
        validate_body(std::dynamic_pointer_cast<BODYBranch>(branch));
    }
    else if (type == "V_DEF")
    {
        validate_vdef(std::dynamic_pointer_cast<VDEFBranch>(branch));
    }
    else if (type == "STRUCT_DEF")
    {
        validate_structure_definition(std::dynamic_pointer_cast<STRUCTDEFBranch>(branch));
    }
    else if (type == "STRUCT")
    {
        validate_structure(std::dynamic_pointer_cast<STRUCTBranch>(branch));
    }
    else if (type == "VAR_IDENTIFIER")
    {
        validate_var_access(std::dynamic_pointer_cast<VarIdentifierBranch>(branch));
    }
    else if (type == "PTR")
    {
        validate_pointer_access(std::dynamic_pointer_cast<PTRBranch>(branch));
    }
    else if (type == "ASSIGN")
    {
        validate_assignment(std::dynamic_pointer_cast<AssignBranch>(branch));
    }
    else if (type == "FUNC_CALL")
    {
        validate_function_call(std::dynamic_pointer_cast<FuncCallBranch>(branch));
    }
}

void SemanticValidator::validate_function(std::shared_ptr<FuncBranch> func_branch)
{
    // Register the function error logged if function is already registered
    register_function(func_branch);

    // Validate the function arguments
    for (std::shared_ptr<Branch> func_arg_child : func_branch->getArgumentsBranch()->getChildren())
    {
        validate_part(func_arg_child);
    }


    // Validate the function body
    validate_part(func_branch->getBodyBranch());
}

void SemanticValidator::validate_body(std::shared_ptr<BODYBranch> body_branch)
{
    for (std::shared_ptr<Branch> child : body_branch->getChildren())
    {
        validate_part(child);
    }
}

void SemanticValidator::validate_vdef(std::shared_ptr<VDEFBranch> vdef_branch)
{
    std::shared_ptr<VarIdentifierBranch> vdef_var_iden_branch = vdef_branch->getVariableIdentifierBranch();
    std::string vdef_var_iden_name = vdef_var_iden_branch->getVariableNameBranch()->getValue();
    // Check to see if the variable definition is already registered in this scope
    for (std::shared_ptr<Branch> child : vdef_branch->getLocalScope()->getChildren())
    {
        // We check for a cast rather than type as other branches can extend V_DEF branch
        std::shared_ptr<VDEFBranch> vdef_child = std::dynamic_pointer_cast<VDEFBranch>(child);
        if (vdef_child != NULL)
        {
            std::shared_ptr<VarIdentifierBranch> vdef_child_var_iden_branch = vdef_child->getVariableIdentifierBranch();
            std::string vdef_child_var_iden_name = vdef_child_var_iden_branch->getVariableNameBranch()->getValue();
            // Is this the current branch we are validating?
            if (vdef_child == vdef_branch)
                continue;

            if (vdef_var_iden_name == vdef_child_var_iden_name)
            {
                // We already have a variable of this name on this scope
                logger->error("The variable \"" + vdef_var_iden_name + "\" has been redeclared", vdef_branch);
            }
        }
    }

    // Lets validate the value if one exists
    if (vdef_branch->hasValueExpBranch())
    {
        validate_value(vdef_branch->getValueExpBranch(), vdef_branch->getDataTypeBranch()->getDataType(), vdef_branch->isPointer(), vdef_branch->getPointerDepth());
    }
}

void SemanticValidator::validate_var_access(std::shared_ptr<VarIdentifierBranch> var_iden_branch)
{
    // We need to check that the var identifier links to a valid variable.
    std::shared_ptr<VDEFBranch> root_vdef_branch = var_iden_branch->getVariableDefinitionBranch(true);

    if (root_vdef_branch == NULL)
    {
        // No V_DEF branch exists for this var identifier branch so it is illegal
        logger->error("The variable \"" + var_iden_branch->getVariableNameBranch()->getValue() + "\" could not be found", var_iden_branch);
    }
    else if (root_vdef_branch->getType() == "STRUCT_DEF"
            && var_iden_branch->hasStructureAccessBranch())
    {
        std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(root_vdef_branch);
        std::string struct_name = struct_def_branch->getDataTypeBranch()->getDataType();
        // This is a structure definition so far the root of the structure is valid but its access may not be 

        std::shared_ptr<STRUCTBranch> struct_branch = this->tree->getGlobalStructureByName(struct_name);
        std::shared_ptr<VarIdentifierBranch> current = var_iden_branch->getStructureAccessBranch()->getVarIdentifierBranch();
        while (true)
        {
            std::shared_ptr<Branch> child = NULL;
            for (std::shared_ptr<Branch> c : struct_branch->getStructBodyBranch()->getChildren())
            {
                if (c->getBranchType() == BRANCH_TYPE_VDEF)
                {
                    std::shared_ptr<VDEFBranch> c_vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(c);
                    if (c_vdef_branch->getNameBranch()->getValue() == current->getVariableNameBranch()->getValue())
                        child = c_vdef_branch;
                }
            }
            if (child == NULL)
            {
                // The variable on the structure does not exist.
                this->logger->error("The variable \"" + current->getVariableNameBranch()->getValue()
                                    + "\" does not exist in structure \"" + struct_branch->getStructNameBranch()->getValue() + "\"", current);
                break;
            }

            if (current->hasStructureAccessBranch())
            {
                struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(child);
                std::string struct_name = struct_def_branch->getDataTypeBranch()->getDataType();
                // Check that the structure is declared
                if (!this->tree->isGlobalStructureDeclared(struct_name))
                {
                    this->logger->error("The structure \"" + struct_name + "\" does not exist", current);
                    break;
                }
                else
                {
                    // Its declared so assign it for later
                    struct_branch = this->tree->getGlobalStructureByName(struct_name);
                }

                // Change the current variable in question
                current = current->getStructureAccessBranch()->getVarIdentifierBranch();
            }
            else
            {
                break;
            }
        }
    }
}

/**
 * Attempts to validate the pointer branch
 * 
 * @param ptr_branch
 * @return  Returns true on success and false on failure 
 */
bool SemanticValidator::validate_pointer_access(std::shared_ptr<PTRBranch> ptr_branch)
{
    // Lets make sure the pointer access is linking to a valid pointer definition
    if (ptr_branch->getPointerVariableIdentifierBranch() == NULL)
    {
        this->logger->error("Attempting to access invalid pointer value, pointer access must use a valid pointer variable", ptr_branch);
        return false;
    }

    return true;
}

void SemanticValidator::validate_assignment(std::shared_ptr<AssignBranch> assign_branch)
{
    // Validate the variable to assign
    validate_part(assign_branch->getVariableToAssignBranch());

    std::shared_ptr<VarIdentifierBranch> var_iden_branch = NULL;
    std::shared_ptr<Branch> var_to_assign_branch = assign_branch->getVariableToAssignBranch();
    if (var_to_assign_branch->getType() == "PTR")
    {
        std::shared_ptr<PTRBranch> ptr_branch = std::dynamic_pointer_cast<PTRBranch>(var_to_assign_branch);
        // Lets validate the pointer to make sure its all valid
        if (!validate_pointer_access(ptr_branch))
        {
            return;
        }

        // Ok its a pointer branch so get the pointer variable identifier associated with it.
        var_iden_branch = ptr_branch->getPointerVariableIdentifierBranch();
    }
    else
    {
        var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_to_assign_branch);
    }

    std::shared_ptr<VDEFBranch> vdef_branch = var_iden_branch->getVariableDefinitionBranch();
    std::string data_type = vdef_branch->getDataTypeBranch()->getDataType();

    // Validate that the value is legal
    validate_value(assign_branch->getValueBranch(), data_type, vdef_branch->isPointer(), vdef_branch->getPointerDepth());
}

void SemanticValidator::validate_function_call(std::shared_ptr<FuncCallBranch> func_call_branch)
{
    std::string func_name = func_call_branch->getFuncNameBranch()->getValue();
    // Validate the function exists
    ensure_function_exists(func_name, func_call_branch);

    // Validate that all function call parameters are compatible with the given function
    std::shared_ptr<FuncDefBranch> func_branch = getFunction(func_name);
    std::shared_ptr<Branch> func_arguments = func_branch->getArgumentsBranch();
    std::shared_ptr<Branch> func_call_params = func_call_branch->getFuncParamsBranch();
    std::vector<std::shared_ptr < Branch>> func_arguments_children = func_arguments->getChildren();
    std::vector<std::shared_ptr < Branch>> func_call_params_children = func_call_params->getChildren();

    int func_total_args = func_arguments_children.size();
    int func_call_total_params = func_call_params_children.size();
    // First we check that the argument lengths are the same
    if (func_call_total_params != func_total_args)
    {
        this->logger->error("Function call to function \"" + func_name + "\" has an invalid number of arguments. Expecting " + std::to_string(func_total_args) + " arguments but " + std::to_string(func_call_total_params) + " was given");
    }
    else
    {
        // Ok we must check that all function arguments are valid
        for (int i = 0; i < func_call_total_params; i++)
        {
            std::shared_ptr<VDEFBranch> func_argument_vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(func_arguments_children[i]);
            validate_value(
                           func_call_params_children[i],
                           func_argument_vdef_branch->getDataTypeBranch()->getDataType(),
                           func_argument_vdef_branch->isPointer(),
                           func_argument_vdef_branch->getPointerDepth()
                           );
        }
    }
}

void SemanticValidator::validate_structure_definition(std::shared_ptr<STRUCTDEFBranch> struct_def_branch)
{
    // Validate the structure type
    std::string struct_data_type = struct_def_branch->getDataTypeBranch()->getDataType();
    if (!this->tree->isGlobalStructureDeclared(struct_data_type))
    {
        this->logger->error("The structure variable has an illegal type of \"" + struct_data_type + "\"", struct_def_branch);
    }
    // Validate the V_DEF as all STRUCT_DEF's are V_DEF's
    validate_vdef(struct_def_branch);

}

void SemanticValidator::validate_structure(std::shared_ptr<STRUCTBranch> structure_branch)
{
    std::string struct_name = structure_branch->getStructNameBranch()->getValue();
    // Lets count all the structures who have the structure name
    int total = this->tree->root->count_children("STRUCT", [&](std::shared_ptr<Branch> branch) -> bool
    {
        std::shared_ptr<STRUCTBranch> struct_branch = std::dynamic_pointer_cast<STRUCTBranch>(branch);
        if (struct_name == struct_branch->getStructNameBranch()->getValue())
        {
                                                 return true;
        }

        return false;
    });

    if (total > 1)
    {
        this->logger->error("The structure \"" + struct_name + "\" has been redeclared", structure_branch);
    }

    // Validate the structure body
    validate_body(structure_branch->getStructBodyBranch());
}

void SemanticValidator::validate_expression(std::shared_ptr<EBranch> e_branch, std::string requirement_type, bool requires_pointer, int pointer_depth)
{
    std::shared_ptr<Branch> left = e_branch->getFirstChild();
    std::shared_ptr<Branch> right = e_branch->getSecondChild();
    if (left->getType() == "E")
    {
        validate_expression(std::dynamic_pointer_cast<EBranch>(left), requirement_type, requires_pointer, pointer_depth);
    }
    else
    {
        validate_value(left, requirement_type, requires_pointer, pointer_depth);
    }

    if (right->getType() == "E")
    {
        validate_expression(std::dynamic_pointer_cast<EBranch>(right), requirement_type, requires_pointer, pointer_depth);
    }
    else
    {
        validate_value(right, requirement_type, requires_pointer, pointer_depth);
    }
}

void SemanticValidator::validate_value(std::shared_ptr<Branch> branch, std::string requirement_type, bool requires_pointer, int pointer_depth)
{
    std::string branch_type = branch->getType();
    if (branch_type == "E")
    {
        validate_expression(std::dynamic_pointer_cast<EBranch>(branch), requirement_type, requires_pointer, pointer_depth);
    }
    else if (branch_type == "number")
    {
        if (getCompiler()->isPrimitiveDataType(requirement_type))
        {
            long number = std::stoi(branch->getValue());
            if (!getCompiler()->canFit(requirement_type, getCompiler()->getTypeFromNumber(number)))
            {
                this->logger->warn("Decimal number: \"" + std::to_string(number) + "\" cannot fit into type \"" + requirement_type + "\" and will be capped", branch);
            }
        }
        else
        {
            this->logger->error("The type: \"" + requirement_type + "\" is not a primitive type so cannot have a value of a number", branch);
        }
    }
    else if (branch_type == "VAR_IDENTIFIER")
    {
        // Lets first check if the variable exists
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(branch);
        if (ensure_variable_exists(var_iden_branch))
        {
            // Lets get the type of the variable to see if it can fit into our requirement type
            std::string var_iden_name = var_iden_branch->getVariableNameBranch()->getValue();
            std::shared_ptr<VDEFBranch> vdef_branch = var_iden_branch->getVariableDefinitionBranch();
            std::string vdef_type = vdef_branch->getDataTypeBranch()->getDataType();

            if (vdef_branch->isPointer() && requires_pointer
                    && vdef_branch->getPointerDepth() != pointer_depth)
            {
                this->logger->error("The variable: \"" + var_iden_name + "\" must have a pointer depth of "
                                    + std::to_string(pointer_depth) + " but its pointer depth is: " + std::to_string(vdef_branch->getPointerDepth()));
            }
            if (!vdef_branch->isPointer()
                    && requires_pointer)
            {
                this->logger->warn("The variable: \"" + var_iden_name + "\" is not a pointer but a pointer was expected", branch);
            }
            else if (vdef_branch->isPointer()
                    && !requires_pointer)
            {
                this->logger->warn("The variable: \"" + var_iden_name + "\" is a pointer but a non pointer type is expected");
            }

            if (getCompiler()->isPrimitiveDataType(requirement_type))
            {
                if (!getCompiler()->isPrimitiveDataType(vdef_type))
                {
                    this->logger->error("Expecting a primitive type of type \"" + requirement_type
                                        + "\" but a non primitive type of type \"" + vdef_type + "\" was provided", branch);
                }
            }
            else
            {
                if (!getCompiler()->isPrimitiveDataType(vdef_type))
                {
                    this->logger->error("Expecting a non-primitive type of type \"" + requirement_type
                                        + "\" but a primitive type of type \"" + vdef_type + "\" was provided", branch);
                }
                else
                {
                    // We are checking to see if both structure types match
                    if (requirement_type != vdef_type)
                    {
                        this->logger->error("Expecting a primitive type whose name is \"" + requirement_type + "\" but type \"" + vdef_type + "\" was provided", branch);
                    }
                }
            }
        }
    }
    else if (branch_type == "FUNC_CALL")
    {
        std::shared_ptr<FuncCallBranch> func_call_branch = std::dynamic_pointer_cast<FuncCallBranch>(branch);
        std::string function_name = func_call_branch->getFuncNameBranch()->getValue();
        std::shared_ptr<FuncDefBranch> func_def_branch = func_call_branch->getFunctionDefinitionBranch();
        std::shared_ptr<DataTypeBranch> func_def_return_type_branch = func_def_branch->getReturnDataTypeBranch();
        std::string return_data_type = func_def_return_type_branch->getDataType();
        // Validate the return type is allowed
        if (return_data_type == "void")
        {
            this->logger->error("Function: \"" + function_name + "\" returns type void this is illegal for expressions", func_call_branch);
        }
        else if (requires_pointer && (!func_def_return_type_branch->isPointer() || return_data_type != requirement_type))
        {
            this->logger->error("Function: \"" + function_name + "\" does not return a pointer of type \"" + requirement_type + "\"", func_call_branch);
        }
        else if (!requires_pointer && func_def_return_type_branch->isPointer())
        {
            this->logger->error("Function \"" + function_name + "\" returns a pointer of type \"" + requirement_type + "\" but we are expecting a non pointer type of \"" + requirement_type + "\"", func_call_branch);
        }
        else if (requires_pointer && func_def_return_type_branch->isPointer() && pointer_depth != func_def_return_type_branch->getPointerDepth())
        {
            this->logger->error("Function \"" + function_name + "\" returns a pointer of type \"" + requirement_type + "\" with a pointer depth of " + std::to_string(func_def_return_type_branch->getPointerDepth()) + " but we are expecting a pointer depth of " + std::to_string(pointer_depth), func_call_branch);
        }
        else if (getCompiler()->isPrimitiveDataType(requirement_type) && !getCompiler()->isPrimitiveDataType(return_data_type))
        {
            this->logger->error("Function \"" + function_name + "\" is returning a non-primitive type of type: \"" + return_data_type + "\". Expecting a primitive type of type \"" + requirement_type + "\"", func_call_branch);
        }
        else if (!getCompiler()->isPrimitiveDataType(requirement_type) && getCompiler()->isPrimitiveDataType(return_data_type))
        {
            this->logger->error("Function \"" + function_name + "\" is returning a primitive type of type: \"" + return_data_type + "\". Expecting a non-primitive type of type \"" + requirement_type + "\"", func_call_branch);
        }
        else if (getCompiler()->isPrimitiveDataType(requirement_type) && !getCompiler()->canFit(requirement_type, return_data_type))
        {
            this->logger->warn("Function: \"" + function_name + "\" returns type \"" + return_data_type + "\" but this primitive type cannot fit directly into \"" + requirement_type + "\" data will be lost", func_call_branch);
        }


        // Validate the function call
        validate_function_call(std::dynamic_pointer_cast<FuncCallBranch>(branch));

    }
}

void SemanticValidator::register_function(std::shared_ptr<FuncDefBranch> func_def_branch)
{
    // Validate the function
    std::string func_name = func_def_branch->getNameBranch()->getValue();
    if (hasFunction(func_name))
    {
        critical_error("The function: \"" + func_name + "\" has already been declared but you are attempting to redeclare it", func_def_branch);
    }

    // Register it
    this->functions[func_name] = func_def_branch;
}

bool SemanticValidator::hasFunction(std::string function_name)
{
    return this->functions.find(function_name) != this->functions.end();
}

std::shared_ptr<FuncDefBranch> SemanticValidator::getFunction(std::string function_name)
{
    if (!hasFunction(function_name))
    {
        throw Exception("Function: " + function_name + " has not been found", "std::shared_ptr<FuncDefBranch> getFunction(std::string function_name)");
    }

    return this->functions[function_name];
}

bool SemanticValidator::ensure_function_exists(std::string func_name, std::shared_ptr<Branch> stmt_branch)
{
    if (!hasFunction(func_name))
    {
        function_not_declared(func_name, stmt_branch);
        return false;
    }

    return true;
}

bool SemanticValidator::ensure_variable_exists(std::shared_ptr<VarIdentifierBranch> var_iden_branch)
{
    if (var_iden_branch->getVariableDefinitionBranch() == NULL)
    {
        variable_not_declared(var_iden_branch);
        return false;
    }

    return true;
}

void SemanticValidator::function_not_declared(std::string func_name, std::shared_ptr<Branch> stmt_branch)
{
    this->logger->error("The function \"" + func_name + "\" is not declared but you are trying to use it or get its address", stmt_branch);
}

void SemanticValidator::variable_not_declared(std::shared_ptr<VarIdentifierBranch> var_iden_branch)
{
    std::string var_name = var_iden_branch->getVariableNameBranch()->getValue();
    this->logger->error("The variable \"" + var_name + "\" is not declared", var_iden_branch);
}

void SemanticValidator::critical_error(std::string message, std::shared_ptr<Branch> branch)
{
    this->logger->error(message, std::dynamic_pointer_cast<CustomBranch>(branch));
    throw Exception("Critical Error!");
}

std::shared_ptr<Logger> SemanticValidator::getLogger()
{
    return this->logger;
}
