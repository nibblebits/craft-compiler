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
 * File:   TreeImprover.cppim
 * Author: Daniel McCarthy
 *
 * Created on 10 December 2016, 12:52
 * 
 * Description: Improves the tree by removing unnecessary branches, for example 50 + 40 would become one branch with value 90.
 * Things that cannot be done in parsing as the tree has not been validated will also happen, such as letting branches know who they are related to.
 * The TreeImprover will make the tree and its branches more powerful and efficient.
 */

#include "TreeImprover.h"
#include "branches.h"

improvement::improvement()
{
    this->is_in_struct = false;
}

improvement::~improvement()
{

}

void improvement::push_struct_def_branch(std::shared_ptr<STRUCTDEFBranch> struct_def_branch)
{
    this->is_in_struct = true;
    this->nested_struct_branches_im_in.push_back(struct_def_branch);
}

std::shared_ptr<STRUCTDEFBranch> improvement::pop_struct_def_branch()
{
    if (this->nested_struct_branches_im_in.empty())
    {
        throw Exception("Stack is empty.", "std::shared_ptr<STRUCTDEFBranch> improvement::pop_struct_def_branch()");
    }

    std::shared_ptr<STRUCTDEFBranch> struct_def_branch = this->nested_struct_branches_im_in.back();
    this->nested_struct_branches_im_in.pop_back();
    return struct_def_branch;
}

bool improvement::isInStruct(std::string struct_name)
{
    for (std::shared_ptr<STRUCTDEFBranch> struct_def_branch : this->nested_struct_branches_im_in)
    {
        if (struct_def_branch->getDataTypeBranch()->getDataType() == struct_name)
            return true;
    }
    return false;
}

std::shared_ptr<STRUCTDEFBranch> improvement::getStructDefFromStack(std::string struct_def_name)
{
    for (std::shared_ptr<STRUCTDEFBranch> struct_def_branch : this->nested_struct_branches_im_in)
    {
        if (struct_def_branch->getDataTypeBranch()->getDataType() == struct_def_name)
            return struct_def_branch;
    }

    return NULL;
}

TreeImprover::TreeImprover(Compiler* compiler) : CompilerEntity(compiler)
{
    this->current_var_type = VARIABLE_TYPE_UNKNOWN;
}

TreeImprover::~TreeImprover()
{
}

void TreeImprover::setTree(std::shared_ptr<Tree> tree)
{
    this->tree = tree;
}

void TreeImprover::improve()
{
    struct improvement improvement;
    improve_top(&improvement);
}

void TreeImprover::improve_top(struct improvement* improvement)
{
    this->tree->root->iterate_children([&](std::shared_ptr<Branch> child_branch)
    {
        // The top is always a global variable
        this->current_var_type = VARIABLE_TYPE_GLOBAL_VARIABLE;
        improve_branch(child_branch, improvement);
    });
}

void TreeImprover::improve_branch(std::shared_ptr<Branch> branch, struct improvement* improvement)
{
    if (branch->getType() == "FUNC")
    {
        improve_func(std::dynamic_pointer_cast<FuncBranch>(branch), improvement);
    }
    else if (branch->getType() == "FUNC_CALL")
    {
        improve_func_call(std::dynamic_pointer_cast<FuncCallBranch>(branch), improvement);
    }
    else if (branch->getType() == "E")
    {
        improve_expression(std::dynamic_pointer_cast<EBranch>(branch), improvement);
    }
    else if (branch->getType() == "IF")
    {
        improve_if(std::dynamic_pointer_cast<IFBranch>(branch), improvement);
    }
    else if (branch->getType() == "WHILE")
    {
        improve_while(std::dynamic_pointer_cast<WhileBranch>(branch), improvement);
    }
    else if (branch->getType() == "FOR")
    {
        improve_for(std::dynamic_pointer_cast<FORBranch>(branch), improvement);
    }
    else if (branch->getType() == "PTR")
    {
        improve_ptr(std::dynamic_pointer_cast<PTRBranch>(branch), improvement);
    }
    else if (branch->getType() == "ASSIGN")
    {
        std::shared_ptr<AssignBranch> assign_child = std::dynamic_pointer_cast<AssignBranch>(branch);
        std::shared_ptr<Branch> value_branch = assign_child->getValueBranch();
        improve_branch(value_branch, improvement);
        improve_branch(assign_child->getVariableToAssignBranch(), improvement);
    }
    else if (branch->getType() == "RETURN")
    {
        std::shared_ptr<ReturnBranch> return_branch = std::dynamic_pointer_cast<ReturnBranch>(branch);
        if (return_branch->hasExpressionBranch())
        {
            improve_branch(return_branch->getExpressionBranch(), improvement);
        }
    }
    else if (branch->getType() == "ADDRESS_OF")
    {
        std::shared_ptr<AddressOfBranch> address_of_branch = std::dynamic_pointer_cast<AddressOfBranch>(branch);
        improve_branch(address_of_branch->getVariableBranch(), improvement);
    }
    else if (branch->getType() == "VAR_IDENTIFIER")
    {
        improve_var_iden(std::dynamic_pointer_cast<VarIdentifierBranch>(branch), improvement);
    }
    else if (branch->getType() == "STRUCT_DEF")
    {
        std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(branch);
        std::string struct_name = struct_def_branch->getDataTypeBranch()->getDataType();
        std::shared_ptr<STRUCTBranch> struct_branch = std::dynamic_pointer_cast<STRUCTBranch>(this->tree->root->getDeclaredStructureByName(struct_name));
        std::shared_ptr<BODYBranch> struct_branch_body = struct_branch->getStructBodyBranch();
        /* Before improving the STRUCT_DEF it is important to make sure we are not nested already and otherwise
         * push ourself to a stack so that infinite nesting can be avoided*/
        if (!improvement->isInStruct(struct_name))
        {
            /* We need to clone the body of the structure that this structure definition is referring to
             * this is because the framework requires unique children for it to do certain things.
             * Upon cloning we will then let the new structure definition know about it.
             */
            std::shared_ptr<BODYBranch> unique_body = std::dynamic_pointer_cast<BODYBranch>(struct_branch_body->clone());
            struct_def_branch->setStructBody(unique_body);

            // Scopes for struct_declaration are set after pushing the branch.
            // We need to set the local scope and root scope to match that of the structure declaration body's scope
            unique_body->setLocalScope(struct_def_branch->getLocalScope());
            unique_body->setRootScope(struct_def_branch->getRootScope());

            // We now need to set all the unique body's children scopes to point to the unique_body
            unique_body->iterate_children([&](std::shared_ptr<Branch> child_branch)
            {
                child_branch->setLocalScope(unique_body);
                child_branch->setRootScope(unique_body->getRootScope());
            });


            // Set the unique body's parent to our struct declaration
            unique_body->setParent(struct_def_branch);

            // Ok this structure definition has not been defined before so lets push ourself to the stack
            improvement->push_struct_def_branch(struct_def_branch);

            // Now lets process this unique structure body
            improve_body(unique_body, improvement);

            // We are done so lets pop ourself off
            improvement->pop_struct_def_branch();
        }
        else
        {
            // We are already registered, to avoid an infinite loop we should set the information to be that of its prior
            std::shared_ptr<STRUCTDEFBranch> defined_struct_def_branch = improvement->getStructDefFromStack(struct_name);
            struct_def_branch->setLocalScope(defined_struct_def_branch->getStructBody());
            struct_def_branch->setRootScope(defined_struct_def_branch->getRootScope());
            struct_def_branch->setParent(defined_struct_def_branch->getParent());
            struct_def_branch->setStructBody(defined_struct_def_branch->getStructBody());
            
        }
    }


    // Set the variable type if this is a variable definition
    if (branch->getBranchType() == BRANCH_TYPE_VDEF)
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        vdef_branch->setVariableType(this->current_var_type);

        if (vdef_branch->hasValueExpBranch())
        {
            improve_branch(vdef_branch->getValueExpBranch(), improvement);
        }
    }
}

void TreeImprover::improve_func(std::shared_ptr<FuncBranch> func_branch, struct improvement* improvement)
{
    std::shared_ptr<Branch> func_arguments_branch = func_branch->getArgumentsBranch();
    std::shared_ptr<BODYBranch> func_body_branch = std::dynamic_pointer_cast<BODYBranch>(func_branch->getBodyBranch());

    // Improve the function arguments
    this->current_var_type = VARIABLE_TYPE_FUNCTION_ARGUMENT_VARIABLE;
    improve_func_arguments(func_arguments_branch, improvement);

    // Improve the function body
    bool has_return_branch = false;
    this->current_var_type = VARIABLE_TYPE_FUNCTION_VARIABLE;
    improve_body(func_body_branch, improvement, &has_return_branch);

    if (func_branch->getReturnDataTypeBranch()->getValue() == "void"
            && !has_return_branch)
    {
        // No return branch found for function that returns void so we need to add one
        std::shared_ptr<ReturnBranch> return_branch = std::shared_ptr<ReturnBranch>(new ReturnBranch(getCompiler()));
        func_body_branch->addChild(return_branch);
    }
}

void TreeImprover::improve_func_arguments(std::shared_ptr<Branch> func_args_branch, struct improvement* improvement)
{
    func_args_branch->iterate_children([&](std::shared_ptr<Branch> child_branch)
    {
        improve_branch(child_branch, improvement);
    });
}

void TreeImprover::improve_func_call(std::shared_ptr<FuncCallBranch> func_call_branch, struct improvement* improvement)
{
    // We need to improve this function call
    func_call_branch->getFuncParamsBranch()->iterate_children([&](std::shared_ptr<Branch> child_branch)
    {
        improve_branch(child_branch, improvement);
    });
}

void TreeImprover::improve_body(std::shared_ptr<BODYBranch> body_branch, struct improvement* improvement, bool* has_return_branch)
{
    if (has_return_branch != NULL)
    {
        *has_return_branch = false;
    }

    body_branch->iterate_children([&](std::shared_ptr<Branch> child_branch)
    {
        if (child_branch->getType() == "RETURN"
                && has_return_branch != NULL)
        {
            *has_return_branch = true;
        }

        improve_branch(child_branch, improvement);
    });
}

void TreeImprover::improve_var_iden(std::shared_ptr<VarIdentifierBranch> var_iden_branch, struct improvement* improvement)
{
    if (var_iden_branch->hasStructureAccessBranch() && var_iden_branch->hasVariableDefinitionBranch(true))
    {
        // Time to set local scopes for the structure access to point to correct structure scope bodys
        std::shared_ptr<STRUCTAccessBranch> access_branch = std::dynamic_pointer_cast<STRUCTAccessBranch>(var_iden_branch->getStructureAccessBranch());
        std::shared_ptr<VDEFBranch> vdef_branch = var_iden_branch->getVariableDefinitionBranch(true);
        std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(vdef_branch);
        std::shared_ptr<BODYBranch> body_branch = struct_def_branch->getStructBody();
        std::shared_ptr<VarIdentifierBranch> next_var_iden_branch = access_branch->getVarIdentifierBranch();
        access_branch->setLocalScope(body_branch);
        next_var_iden_branch->setLocalScope(body_branch);
        // Process the VAR_IDENTIFIER below it
        improve_var_iden(next_var_iden_branch, improvement);
    }

    if (var_iden_branch->hasRootArrayIndexBranch())
    {
        std::shared_ptr<ArrayIndexBranch> array_index_branch = var_iden_branch->getRootArrayIndexBranch();
        improve_branch(array_index_branch->getValueBranch(), improvement);
    }
}

void TreeImprover::improve_if(std::shared_ptr<IFBranch> if_branch, struct improvement* improvement)
{
    // Improve the expression of the if statement
    improve_branch(if_branch->getExpressionBranch(), improvement);

    // Improve the body of the if statement
    improve_body(if_branch->getBodyBranch(), improvement);

    if (if_branch->hasElseIfBranch())
    {
        // Improve the else if branch
        improve_if(if_branch->getElseIfBranch(), improvement);
    }

    if (if_branch->hasElseBranch())
    {
        // Improve the body of the else branch
        improve_body(if_branch->getElseBranch()->getBodyBranch(), improvement);
    }
}

void TreeImprover::improve_while(std::shared_ptr<WhileBranch> while_branch, struct improvement* improvement)
{
    // improve the expression of the while loop
    improve_branch(while_branch->getExpressionBranch(), improvement);

    // Improve the body of the while loop
    improve_body(while_branch->getBodyBranch(), improvement);

}

void TreeImprover::improve_for(std::shared_ptr<FORBranch> for_branch, struct improvement* improvement)
{
    improve_branch(for_branch->getInitBranch(), improvement);
    improve_branch(for_branch->getCondBranch(), improvement);
    improve_branch(for_branch->getLoopBranch(), improvement);
    improve_body(for_branch->getBodyBranch(), improvement);
}

void TreeImprover::improve_ptr(std::shared_ptr<PTRBranch> ptr_branch, struct improvement* improvement)
{
    improve_branch(ptr_branch->getExpressionBranch(), improvement);
}

void TreeImprover::improve_expression(std::shared_ptr<EBranch> expression_branch, struct improvement* improvement, bool is_root)
{
    std::shared_ptr<Branch> left_branch = expression_branch->getFirstChild();
    std::shared_ptr<Branch> right_branch = expression_branch->getSecondChild();

    if (left_branch->getType() == "number" &&
            right_branch->getType() == "number")
    {
        // Both are numbers so we can safely craft a new branch from them
        std::shared_ptr<Token> left_token = std::dynamic_pointer_cast<Token>(left_branch);
        // Both the left and right branches contain numbers so lets evaluate the numbers and replace them with one branch holding the result
        int left_n = std::stoi(left_branch->getValue());
        int right_n = std::stoi(right_branch->getValue());
        long result = getCompiler()->evaluate(left_n, right_n, expression_branch->getValue());

        CharPos pos = left_token->getPosition();
        std::shared_ptr<Token> token = std::shared_ptr<Token>(new Token("number", std::to_string(result), pos));
        expression_branch->replaceSelf(token);
    }


    // Improve the left and right expression operand branches
    improve_branch(left_branch, improvement);
    improve_branch(right_branch, improvement);

    // If the branches were replaced on the tree we need to get their new branch.
    if (left_branch->wasReplaced())
    {
        left_branch = left_branch->getReplaceeBranch();
    }
    if (right_branch->wasReplaced())
    {
        right_branch = right_branch->getReplaceeBranch();
    }


    if (right_branch->getType() == "E")
    {
        std::shared_ptr<EBranch> e_right_branch = std::dynamic_pointer_cast<EBranch>(right_branch);
        if (e_right_branch->allAreNumbers())
        {
            // We can go again
            improve_expression(e_right_branch, improvement, false);
        }
    }
    if (left_branch->getType() == "E")
    {
        std::shared_ptr<EBranch> e_left_branch = std::dynamic_pointer_cast<EBranch>(left_branch);
        if (e_left_branch->allAreNumbers())
        {
            // We can go again
            improve_expression(e_left_branch, improvement, false);
        }
    }


    // We need to check to see if our root was replaced because of cases like this: (50 + 23) + (90 + 40) we will still have an E branch when there does not need to be one
    if (is_root)
    {
        if (!expression_branch->wasReplaced())
        {
            if (expression_branch->allAreNumbers())
            {
                improve_expression(expression_branch, improvement, false);
            }
            else
            {
                // We need to check for numbers that can be added together to further shrink the expression
                if (expression_branch->hasOnlyOneNumber()
                        && expression_branch->hasOnlyOneExpression())
                {
                    std::shared_ptr<Token> number_branch = expression_branch->getOnlyNumberBranch();
                    std::shared_ptr<EBranch> exp_branch = expression_branch->getOnlyExpressionBranch();

                    // We can only reduct if the expressions share the same operator
                    if (expression_branch->getValue() == exp_branch->getValue())
                    {
                        // Sub expressions eligible for this reduction will only have one number branch as if they had two they would have been reduced earlier
                        if (exp_branch->hasOnlyOneNumber())
                        {
                            std::shared_ptr<Token> second_number_branch = exp_branch->getOnlyNumberBranch();
                            // Ok these branches are eligible for reduction lets evaluate their values
                            long new_value = getCompiler()->evaluate(
                                                                     std::stoi(number_branch->getValue()),
                                                                     std::stoi(second_number_branch->getValue()),
                                                                     expression_branch->getValue());
                            // We now have the new value so we should replace the second number with the result and then remove the first number
                            std::shared_ptr<Token> new_token = std::shared_ptr<Token>(new Token("number", std::to_string(new_value), number_branch->getPosition()));
                            second_number_branch->replaceSelf(new_token);

                            // Finally we need to remove the first number from the tree and rebuild both expression branches
                            number_branch->removeSelf();

                            expression_branch->rebuild();
                            exp_branch->rebuild();
                        }
                    }

                }
            }
        }
    }
}
