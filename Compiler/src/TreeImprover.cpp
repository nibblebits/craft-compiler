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
    for (std::shared_ptr<Branch> child : this->tree->root->getChildren())
    {
        if (child->getType() == "FUNC")
        {
            improve_func(std::dynamic_pointer_cast<FuncBranch>(child));
        }
    }
}

void TreeImprover::improve_func(std::shared_ptr<FuncBranch> func_branch)
{
    std::shared_ptr<BODYBranch> func_body_branch = std::dynamic_pointer_cast<BODYBranch>(func_branch->getBodyBranch());
    improve_body(func_body_branch);
}

void TreeImprover::improve_body(std::shared_ptr<BODYBranch> body_branch)
{

    for (std::shared_ptr<Branch> child : body_branch->getChildren())
    {
        if (child->getType() == "ASSIGN")
        {
            std::shared_ptr<AssignBranch> assign_child = std::dynamic_pointer_cast<AssignBranch>(child);
            std::shared_ptr<Branch> value_branch = assign_child->getValueBranch();
            improve_expression(value_branch);
            improve_var_iden(std::dynamic_pointer_cast<VarIdentifierBranch>(assign_child->getVariableToAssignBranch()));
        }
        else if (child->getType() == "STRUCT_DEF")
        {
            /* We need to clone the body of the structure that this structure definition is referring to
             * this is because the framework requires unique children for it to do certain things.
             * Upon cloning we will then let the new structure definition know about it.
             */
            std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(child);
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
            for (std::shared_ptr<Branch> child : unique_body->getChildren())
            {
                child->setLocalScope(unique_body);
                child->setRootScope(unique_body->getRootScope());
            }

            // Set the unique body's parent to our struct declaration
            unique_body->setParent(struct_def_branch);
            
            // Now lets process this unique structure body
            improve_body(unique_body);
        }
    }
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