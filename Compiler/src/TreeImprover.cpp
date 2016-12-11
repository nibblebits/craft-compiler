/*
    Goblin compiler v1.0 - The standard compiler for the Goblin language.
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
 * File:   TreeImprover.cpp
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
    improve_top();
}

void TreeImprover::improve_top()
{
    this->tree->root->iterate_children([&](std::shared_ptr<Branch> child_branch)
    {
        // The top is always a global variable
        this->current_var_type = VARIABLE_TYPE_GLOBAL_VARIABLE;
        improve_branch(child_branch);
    });
}

void TreeImprover::improve_branch(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "FUNC")
    {
        improve_func(std::dynamic_pointer_cast<FuncBranch>(branch));
    }
    else if (branch->getType() == "IF")
    {
        improve_if(std::dynamic_pointer_cast<IFBranch>(branch));
    }
    else if (branch->getType() == "FOR")
    {
        improve_for(std::dynamic_pointer_cast<FORBranch>(branch));
    }
    else if (branch->getType() == "ASSIGN")
    {
        std::shared_ptr<AssignBranch> assign_child = std::dynamic_pointer_cast<AssignBranch>(branch);
        std::shared_ptr<Branch> value_branch = assign_child->getValueBranch();
        improve_expression(value_branch);
        improve_var_iden(std::dynamic_pointer_cast<VarIdentifierBranch>(assign_child->getVariableToAssignBranch()));
    }
    else if (branch->getType() == "STRUCT_DEF")
    {
        /* We need to clone the body of the structure that this structure definition is referring to
         * this is because the framework requires unique children for it to do certain things.
         * Upon cloning we will then let the new structure definition know about it.
         */
        std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(branch);
        std::shared_ptr<Branch> struct_name_branch = struct_def_branch->getDataTypeBranch();
        std::shared_ptr<STRUCTBranch> struct_branch = std::dynamic_pointer_cast<STRUCTBranch>(this->tree->root->getDeclaredStructureByName(struct_name_branch->getValue()));
        std::shared_ptr<BODYBranch> struct_branch_body = struct_branch->getStructBodyBranch();
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

        // Now lets process this unique structure body
        improve_body(unique_body);
    }

    // Set the variable type if this is a variable definition
    if (branch->getBranchType() == BRANCH_TYPE_VDEF)
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        vdef_branch->setVariableType(this->current_var_type);
    }
}

void TreeImprover::improve_func(std::shared_ptr<FuncBranch> func_branch)
{
    std::shared_ptr<Branch> func_arguments_branch = func_branch->getArgumentsBranch();
    std::shared_ptr<BODYBranch> func_body_branch = std::dynamic_pointer_cast<BODYBranch>(func_branch->getBodyBranch());

    // Improve the function arguments
    this->current_var_type = VARIABLE_TYPE_FUNCTION_ARGUMENT_VARIABLE;
    improve_func_arguments(func_arguments_branch);


    // Improve the function body
    this->current_var_type = VARIABLE_TYPE_FUNCTION_VARIABLE;
    improve_body(func_body_branch);
}

void TreeImprover::improve_func_arguments(std::shared_ptr<Branch> func_args_branch)
{
    func_args_branch->iterate_children([&](std::shared_ptr<Branch> child_branch)
    {
        improve_branch(child_branch);
    });
}

void TreeImprover::improve_body(std::shared_ptr<BODYBranch> body_branch)
{
    body_branch->iterate_children([&](std::shared_ptr<Branch> child_branch)
    {
        improve_branch(child_branch);
    });
}

void TreeImprover::improve_expression(std::shared_ptr<Branch> expression_branch)
{
    // This will do barley anything for now as I plan to change the design of expressions in the future, no point wasting precious time.
    if (expression_branch->getType() == "VAR_IDENTIFIER")
    {
        improve_var_iden(std::dynamic_pointer_cast<VarIdentifierBranch>(expression_branch));
    }
}

void TreeImprover::improve_var_iden(std::shared_ptr<VarIdentifierBranch> var_iden_branch)
{
    if (var_iden_branch->hasStructureAccessBranch())
    {
        // Time to set local scopes for the structure access to point to correct structure scope bodys
        std::shared_ptr<STRUCTAccessBranch> access_branch = std::dynamic_pointer_cast<STRUCTAccessBranch>(var_iden_branch->getStructureAccessBranch());
        std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(var_iden_branch->getVariableDefinitionBranch(true));
        std::shared_ptr<BODYBranch> body_branch = struct_def_branch->getStructBody();
        std::shared_ptr<VarIdentifierBranch> next_var_iden_branch = access_branch->getVarIdentifierBranch();
        access_branch->setLocalScope(body_branch);
        next_var_iden_branch->setLocalScope(body_branch);
        // Process the VAR_IDENTIFIER below it
        improve_var_iden(next_var_iden_branch);
    }
}

void TreeImprover::improve_if(std::shared_ptr<IFBranch> if_branch)
{
    // Improve the body of the if statement
    improve_body(if_branch->getBodyBranch());

    if (if_branch->hasElseIfBranch())
    {
        // Improve the body of the else if branch
        improve_body(if_branch->getElseIfBranch()->getBodyBranch());
    }

    if (if_branch->hasElseBranch())
    {
        // Improve the body of the else branch
        improve_body(if_branch->getElseBranch()->getBodyBranch());
    }
}

void TreeImprover::improve_for(std::shared_ptr<FORBranch> for_branch)
{
    
}