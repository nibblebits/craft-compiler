/*
    Craft compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   Preprocessor.cpp
 * Author: Daniel McCarthy
 *
 * Created on 08 January 2017, 01:58
 * 
 * Description: 
 */

#include <ctype.h>

#include "Preprocessor.h"
#include "Tree.h"
#include "branches.h"

Preprocessor::Preprocessor(Compiler* compiler) : CompilerEntity(compiler)
{
    this->logger = std::shared_ptr<Logger>(new Logger());
    // Standard macro functions
    register_macro_function("sizeof", 1, [&](std::shared_ptr<Branch> args) -> int
    {
        int result;
        // Ok we need to get the size of the element
        std::shared_ptr<Branch> argument = args->getFirstChild();
        std::string argument_type = argument->getType();
        if (argument_type == "VAR_IDENTIFIER")
        {
                            std::shared_ptr<VarIdentifierBranch> variable = std::dynamic_pointer_cast<VarIdentifierBranch>(argument);
                            std::shared_ptr<VDEFBranch> vdef_branch = variable->getVariableDefinitionBranch();
                            result = vdef_branch->getDataTypeBranch()->getDataTypeSize();
        }
        else if (argument_type == "STRUCT_DESCRIPTOR")
        {
                            std::shared_ptr<STRUCTDescriptorBranch> struct_descriptor_branch = std::dynamic_pointer_cast<STRUCTDescriptorBranch>(argument);
                            std::shared_ptr<STRUCTBranch> struct_branch = tree->getGlobalStructureByName(struct_descriptor_branch->getStructNameBranch()->getValue());
                            result = struct_branch->getStructBodyBranch()->getScopeSize();
        }
        else if (argument_type == "keyword")
        {
                            // Ok this is just a keyword so we will get the primitive type
                            result = getCompiler()->getPrimitiveDataTypeSize(argument->getValue());
        }

        return result;
    });
}

Preprocessor::~Preprocessor()
{
}

void Preprocessor::setTree(std::shared_ptr<Tree> tree)
{
    this->tree = tree;
}

void Preprocessor::register_macro_function(std::string function_name, int max_args, std::function<int(std::shared_ptr<Branch> branch) > function)
{
    if (is_macro_function_registered(function_name))
    {
        throw Exception("The macro function: " + function_name + " is already registered", "void Preprocessor::register_macro_function(std::string function_name, std::function<int>(std::shared_ptr<Branch> branch))");
    }

    struct macro_function func;
    func.func_name = function_name;
    func.max_args = max_args;
    func.function = function;
    this->macro_functions[function_name] = func;
}

int Preprocessor::invoke_macro_function(std::string function_name, std::shared_ptr<Branch> args)
{
    struct macro_function func = get_macro_function(function_name);
    if (func.max_args != MACRO_FUNCTION_ARGUMENTS_NO_LIMIT
            && args->getChildren().size() > func.max_args)
    {
        throw Exception("Attempting to apply more arguments than allowed for macro function: " + function_name, "void Preprocessor::invoke_macro_function(std::string function_name, std::shared_ptr<Branch> args)");
    }

    // Ok this function call it legit lets invoke it
    int result = func.function(args);
    return result;
}

struct macro_function Preprocessor::get_macro_function(std::string function_name)
{
    if (!is_macro_function_registered(function_name))
    {
        throw Exception("The macro function: " + function_name + " is not registered", "struct macro_function Preprocessor::get_macro_function(std::string function_name)");
    }

    return this->macro_functions[function_name];
}

bool Preprocessor::is_macro_function_registered(std::string function_name)
{
    return macro_functions.find(function_name) != macro_functions.end();
}

void Preprocessor::process()
{
    std::shared_ptr<Branch> root = this->tree->root;
    for (std::shared_ptr<Branch> child : root->getChildren())
    {
        process_child(child);
    }
}

bool Preprocessor::is_macro(std::string macro_name)
{
    return (
            macro_name == "MACRO_IFDEF" ||
            macro_name == "MACRO_IFNDEF" ||
            macro_name == "MACRO_DEFINE" ||
            macro_name == "MACRO_DEFINITION_IDENTIFIER" ||
            macro_name == "MACRO_FUNC_CALL"
            );
}

bool Preprocessor::is_definition_registered(std::string definition_name)
{
    return (this->definitions.find(definition_name) != this->definitions.end());
}

void Preprocessor::define_definition(std::string definition_name, std::string value, PREPROCESSOR_DEF_TYPE def_type)
{
    struct preprocessor_def definition;
    definition.name = definition_name;
    definition.value = value;
    definition.type = def_type;
    this->definitions[definition_name] = definition;
}

std::string Preprocessor::get_definition_value(std::string definition_name)
{
    if (!is_definition_registered(definition_name))
    {
        // Will change this to use error routines like shown in parser, this place holder will work for now.
        throw Exception("std::string Preprocessor::get_definition_value(std::string definition_name): the definition does not exist");
    }

    return this->definitions[definition_name].value;
}

struct preprocessor_def Preprocessor::get_definition(std::string definition_name)
{
    if (!is_definition_registered(definition_name))
    {
        throw Exception("The definition: " + definition_name + " is not registered", "struct preprocessor_def Preprocessor::get_definition(std::string definition_name)");
    }

    return this->definitions[definition_name];
}

std::shared_ptr<Logger> Preprocessor::getLogger()
{
    return this->logger;
}

void Preprocessor::process_macro(std::shared_ptr<Branch> macro)
{
    if (macro->getType() == "MACRO_IFDEF")
    {
        std::shared_ptr<MacroIfDefBranch> macro_ifdef_branch = std::dynamic_pointer_cast<MacroIfDefBranch>(macro);
        process_macro_ifdef(macro_ifdef_branch);
    }
    else if (macro->getType() == "MACRO_IFNDEF")
    {
        std::shared_ptr<MacroIfNDefBranch> macro_ifndef_branch = std::dynamic_pointer_cast<MacroIfNDefBranch>(macro);
        process_macro_ifndef(macro_ifndef_branch);
    }
    else if (macro->getType() == "MACRO_DEFINE")
    {
        std::shared_ptr<MacroDefineBranch> macro_define_branch = std::dynamic_pointer_cast<MacroDefineBranch>(macro);
        process_macro_define(macro_define_branch);
    }
    else if (macro->getType() == "MACRO_DEFINITION_IDENTIFIER")
    {
        std::shared_ptr<MacroDefinitionIdentifierBranch> macro_def_iden_branch = std::dynamic_pointer_cast<MacroDefinitionIdentifierBranch>(macro);
        process_macro_def_identifier(macro_def_iden_branch);
    }
    else if (macro->getType() == "MACRO_FUNC_CALL")
    {
        std::shared_ptr<MacroFuncCallBranch> macro_func_call = std::dynamic_pointer_cast<MacroFuncCallBranch>(macro);
        process_macro_func_call(macro_func_call);
    }
}

void Preprocessor::process_child(std::shared_ptr<Branch> child)
{
    std::string child_type = child->getType();
    if (is_macro(child_type))
    {
        process_macro(child);
    }
    else if (child_type == "FUNC")
    {
        process_func(std::dynamic_pointer_cast<FuncBranch>(child));
    }
    else if (child_type == "BODY")
    {
        process_body(std::dynamic_pointer_cast<BODYBranch>(child));
    }
    else if (child_type == "V_DEF")
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(child);
        process_child(vdef_branch->getVariableIdentifierBranch());
        if (vdef_branch->hasValueExpBranch())
        {
            process_expression(vdef_branch->getValueExpBranch());
        }
    }
    else if (child_type == "ASSIGN")
    {
        std::shared_ptr<AssignBranch> assign_branch = std::dynamic_pointer_cast<AssignBranch>(child);
        process_child(assign_branch->getVariableToAssignBranch());
        process_expression(assign_branch->getValueBranch());
    }
    else if (child_type == "VAR_IDENTIFIER")
    {
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(child);
        if (var_iden_branch->hasRootArrayIndexBranch())
        {
            process_child(var_iden_branch->getRootArrayIndexBranch());
        }

        if (var_iden_branch->hasStructureAccessBranch())
        {
            process_child(var_iden_branch->getStructureAccessBranch());
        }
    }
    else if (child_type == "ARRAY_INDEX")
    {
        std::shared_ptr<ArrayIndexBranch> array_index_branch = std::dynamic_pointer_cast<ArrayIndexBranch>(child);
        process_expression(array_index_branch->getValueBranch());

        // More to go?
        if (array_index_branch->hasNextArrayIndexBranch())
        {
            process_child(array_index_branch->getNextArrayIndexBranch());
        }
    }
    else if (child_type == "PTR")
    {
        std::shared_ptr<PTRBranch> ptr_branch = std::dynamic_pointer_cast<PTRBranch>(child);
        process_expression(ptr_branch->getExpressionBranch());
    }
    else if (child_type == "STRUCT_ACCESS")
    {
        std::shared_ptr<STRUCTAccessBranch> struct_access_branch = std::dynamic_pointer_cast<STRUCTAccessBranch>(child);
        process_child(struct_access_branch->getVarIdentifierBranch());
    }
    else if (child_type == "FUNC_CALL")
    {
        std::shared_ptr<FuncCallBranch> func_call_branch = std::dynamic_pointer_cast<FuncCallBranch>(child);
        for (std::shared_ptr<Branch> branch : func_call_branch->getFuncParamsBranch()->getChildren())
        {
            process_expression(branch);
        }
    }


}

void Preprocessor::process_func(std::shared_ptr<FuncBranch> func_branch)
{
    // Process the function body
    std::shared_ptr<BODYBranch> body_branch = func_branch->getBodyBranch();
    process_body(body_branch);

}

void Preprocessor::process_expression(std::shared_ptr<Branch> child)
{
    if (child->getType() == "E")
    {
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(child);
        std::shared_ptr<Branch> left = e_branch->getFirstChild();
        std::shared_ptr<Branch> right = e_branch->getSecondChild();
        process_expression(left);
        process_expression(right);
    }
    else
    {
        process_expression_part(child);
    }
}

void Preprocessor::process_expression_part(std::shared_ptr<Branch> child)
{
    process_child(child);
}

void Preprocessor::process_body(std::shared_ptr<BODYBranch> body_branch)
{
    for (std::shared_ptr<Branch> child : body_branch->getChildren())
    {
        process_child(child);
    }
}

void Preprocessor::process_macro_ifdef(std::shared_ptr<MacroIfDefBranch> macro_ifdef_branch)
{
    std::shared_ptr<Branch> req_branch = macro_ifdef_branch->getRequirementBranch();
    std::shared_ptr<BODYBranch> body_branch = macro_ifdef_branch->getBodyBranch();

    if (is_definition_registered(req_branch->getValue()))
    {
        // The definition is registered so replace this macro with the body branch
        process_child(body_branch);
        macro_ifdef_branch->replaceSelf(body_branch);
        body_branch->replaceWithChildren();
    }
    else
    {
        // All were false so lets delete ourself and all children associated with us
        macro_ifdef_branch->removeSelf();
    }

}

void Preprocessor::process_macro_ifndef(std::shared_ptr<MacroIfNDefBranch> macro_ifndef_branch)
{
    std::shared_ptr<Branch> req_branch = macro_ifndef_branch->getRequirementBranch();
    std::shared_ptr<BODYBranch> body_branch = macro_ifndef_branch->getBodyBranch();

    if (!is_definition_registered(req_branch->getValue()))
    {
        // The definition is registered so replace this macro with the body branch
        process_child(body_branch);
        macro_ifndef_branch->replaceSelf(body_branch);
        body_branch->replaceWithChildren();
    }
    else
    {
        // All were false so lets delete ourself and all children associated with us
        macro_ifndef_branch->removeSelf();
    }
}

void Preprocessor::process_macro_define(std::shared_ptr<MacroDefineBranch> macro_define_branch)
{
    std::shared_ptr<Branch> def_name_branch = macro_define_branch->getDefinitionNameBranch();
    std::string value_str = "";
    PREPROCESSOR_DEF_TYPE def_type = PREPROCESSOR_DEFINITION_TYPE_UNKNOWN;
    if (macro_define_branch->hasDefinitionValueBranch())
    {
        // Ok this definition has a value so lets evaluate it
        std::shared_ptr<Branch> def_value_exp_branch = macro_define_branch->getDefinitionValueBranch();
        value_str = evaluate_expression(def_value_exp_branch, &def_type);
    }

    // Define this definition
    define_definition(def_name_branch->getValue(), value_str, def_type);

    // Finally lets delete this define branch, we do not want the code generator to see this
    macro_define_branch->removeSelf();
}

void Preprocessor::process_macro_def_identifier(std::shared_ptr<MacroDefinitionIdentifierBranch> macro_def_iden_branch)
{
    std::shared_ptr<Token> iden_token = std::dynamic_pointer_cast<Token>(macro_def_iden_branch->getIdentifierBranch());
    std::string def_name = macro_def_iden_branch->getIdentifierBranch()->getValue();
    if (!is_definition_registered(def_name))
    {
        throw Exception("The definition is not registered: " + def_name, "void Preprocessor::process_macro_def_identifier(std::shared_ptr<MacroDefinitionIdentifierBranch> macro_def_iden_branch)");
    }

    struct preprocessor_def definition = get_definition(def_name);
    std::string token_type;
    if (definition.type == PREPROCESSOR_DEFINITION_TYPE_NUMBER)
    {
        token_type = "number";
    }
    else if (definition.type == PREPROCESSOR_DEFINITION_TYPE_STRING)
    {
        token_type = "string";
    }
    else
    {
        throw Exception("Do not know how to handle definition type of decimal value: " + std::to_string(definition.type), "void Preprocessor::process_macro_def_identifier(std::shared_ptr<MacroDefinitionIdentifierBranch> macro_def_iden_branch)");
    }

    std::shared_ptr<Token> token_to_replace_with = std::shared_ptr<Token>(new Token(token_type, definition.value, iden_token->getPosition()));
    macro_def_iden_branch->replaceSelf(token_to_replace_with);
}

void Preprocessor::process_macro_func_call(std::shared_ptr<MacroFuncCallBranch> macro_func_call_branch)
{
    std::string func_name = macro_func_call_branch->getFuncNameBranch()->getValue();
    int result = invoke_macro_function(func_name, macro_func_call_branch->getFuncParamsBranch());

    // We need to get the closest token that we can so we can get the previous token position. In our case it will be the function name
    std::shared_ptr<Token> token = std::dynamic_pointer_cast<Token>(macro_func_call_branch->getFuncNameBranch());
    std::shared_ptr<Token> token_to_replace_with = std::shared_ptr<Token>(new Token("number", std::to_string(result), token->getPosition()));
    macro_func_call_branch->replaceSelf(token_to_replace_with);
}

std::string Preprocessor::evaluate_expression(std::shared_ptr<Branch> value_branch, PREPROCESSOR_DEF_TYPE* def_type_ptr)
{
    std::string result = "";
    if (value_branch->getType() == "E")
    {
        std::string left_value, right_value, op;
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(value_branch);
        std::shared_ptr<Branch> left_branch = e_branch->getFirstChild();
        std::shared_ptr<Branch> right_branch = e_branch->getSecondChild();
        op = e_branch->getValue();

        if (left_branch->getType() == "E")
        {
            left_value = evaluate_expression(left_branch, def_type_ptr);
        }
        else
        {
            left_value = evaluate_expression_part(left_branch, def_type_ptr);
        }

        if (right_branch->getType() == "E")
        {
            right_value = evaluate_expression(right_branch, def_type_ptr);
        }
        else
        {
            right_value = evaluate_expression_part(right_branch, def_type_ptr);
        }

        /* At this point the left_value and right_value variables should contain strings to append or add together 
         * we need to figure out if we should add these strings together assuming they represent numeric values, or append them 
         * which will be the case if one or both strings are non-numeric.*/

        if (left_branch->getType() == "number"
                && right_branch->getType() == "number")
        {
            // Ok both are numeric we can evaluate them
            result = std::to_string(getCompiler()->evaluate(std::stoi(left_value), std::stoi(right_value), op));
        }
        else
        {
            bool has_processed = false;
            if (left_branch->getType() == "identifier"
                    && right_branch->getType() == "identifier")
            {
                struct preprocessor_def left_def = get_definition(left_branch->getValue());
                struct preprocessor_def right_def = get_definition(right_branch->getValue());
                if (left_def.type == PREPROCESSOR_DEFINITION_TYPE_NUMBER &&
                        right_def.type == PREPROCESSOR_DEFINITION_TYPE_NUMBER)
                {
                    result = std::to_string(getCompiler()->evaluate(std::stoi(left_def.value), std::stoi(right_def.value), op));
                    has_processed = true;
                }
            }
            else if (left_branch->getType() == "identifier"
                    && right_branch->getType() == "number")
            {
                struct preprocessor_def left_def = get_definition(left_branch->getValue());
                if (left_def.type == PREPROCESSOR_DEFINITION_TYPE_NUMBER)
                {
                    result = std::to_string(getCompiler()->evaluate(std::stoi(left_def.value), std::stoi(right_branch->getValue()), op));
                    has_processed = true;
                }
            }
            else if (left_branch->getType() == "number"
                    && right_branch->getType() == "identifier")
            {
                struct preprocessor_def right_def = get_definition(right_branch->getValue());
                if (right_def.type == PREPROCESSOR_DEFINITION_TYPE_NUMBER)
                {
                    result = std::to_string(getCompiler()->evaluate(std::stoi(left_branch->getValue()), std::stoi(right_def.value), op));
                    has_processed = true;
                }
            }

            if (!has_processed)
            {
                // Either the left value or the right value is not completely numeric so we will just append them
                // While appending you must use the "+" operator
                if (op != "+")
                {
                    // Will be replaced to use the error logger that the parser uses 
                    throw Exception("std::string Preprocessor::evaluate_expression(std::shared_ptr<Branch> value_branch): either left value or right value is non numeric, "
                                    "so we tried to append but was expecting operator \"+\" but operator \"" + op + "\" was provided.");
                }

                // Ok lets append them
                result.append(left_value);
                result.append(right_value);
            }
        }
    }
    else
    {
        // Not an expression? no problem lets just evaluate one expression part
        result = evaluate_expression_part(value_branch, def_type_ptr);
    }

    return result;
}

std::string Preprocessor::evaluate_expression_part(std::shared_ptr<Branch> value_branch, PREPROCESSOR_DEF_TYPE* def_type_ptr)
{
    std::string value = "";
    std::string branch_type = value_branch->getType();
    std::string branch_value = value_branch->getValue();
    if (branch_type == "identifier")
    {
        // Ok we need to get the value of this definition name that was provided for the value
        struct preprocessor_def definition = get_definition(branch_value);
        if (*def_type_ptr != PREPROCESSOR_DEFINITION_TYPE_STRING)
        {
            *def_type_ptr = definition.type;
        }

        value = definition.value;
    }
    else if (branch_type == "number"
            || branch_type == "string")
    {
        value = branch_value;

        if (*def_type_ptr != PREPROCESSOR_DEFINITION_TYPE_STRING)
        {
            if (branch_type == "string")
            {
                *def_type_ptr = PREPROCESSOR_DEFINITION_TYPE_STRING;
            }
            else if (branch_type == "number")
            {
                *def_type_ptr = PREPROCESSOR_DEFINITION_TYPE_NUMBER;
            }
        }

    }
    else
    {
        throw Exception("std::string Preprocessor::evaluate_expression_part(std::shared_ptr<Branch> value_branch): expecting either an identifier, string or a number token");
    }

    return value;
}

bool Preprocessor::is_string_numeric_only(std::string str)
{
    for (char c : str)
    {
        if (!isdigit(c) && c != '-')
            return false;
    }

    return true;
}