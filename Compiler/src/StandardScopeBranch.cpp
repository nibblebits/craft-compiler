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
 * File:   StandardScopeBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 15 December 2016, 18:19
 * 
 * Description: The root class for all standard scope branches.
 */

#include "StandardScopeBranch.h"
#include "branches.h"

StandardScopeBranch::StandardScopeBranch(Compiler* compiler, std::string name, std::string value) : ScopeBranch(compiler, name, value)
{
}

StandardScopeBranch::~StandardScopeBranch()
{
}

int StandardScopeBranch::getScopeSize(bool include_subscopes, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_start, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_end, bool *should_stop)
{
    int size = 0;
    for (std::shared_ptr<Branch> child : this->getChildren())
    {
        if (child_proc_start != NULL)
        {
            if (!child_proc_start(child))
            {
                if (should_stop != NULL)
                {
                    *should_stop = true;
                }
                break;
            }
        }

        if (child->getBranchType() == BRANCH_TYPE_VDEF)
        {
            std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(child);
            size += vdef_branch->getSize();

        }
        else if (include_subscopes)
        {
            std::string child_type = child->getType();
            if (child_type == "FOR")
            {
                std::shared_ptr<FORBranch> for_branch = std::dynamic_pointer_cast<FORBranch>(child);
                size += for_branch->getScopeSize(include_subscopes, child_proc_start, child_proc_end, should_stop);
                if (*should_stop)
                {
                    break;
                }
            }
        }

        if (child_proc_end != NULL)
        {
            if (!child_proc_end(child))
            {
                if (should_stop != NULL)
                {
                    *should_stop = true;
                }
                break;
            }
        }
    }
    return size;
}

std::shared_ptr<VDEFBranch> StandardScopeBranch::getVariableDefinitionBranch(std::shared_ptr<VarIdentifierBranch> var_iden, bool lookup_scope, bool no_follow)
{
    std::shared_ptr<VDEFBranch> found_branch = NULL;
    std::shared_ptr<Branch> var_iden_name_branch = var_iden->getVariableNameBranch();
    // Check local scope
    for (std::shared_ptr<Branch> child : this->getChildren())
    {
        if (child->getBranchType() == BRANCH_TYPE_VDEF)
        {
            std::shared_ptr<VDEFBranch> c_vdef = std::dynamic_pointer_cast<VDEFBranch>(child);
            std::shared_ptr<VarIdentifierBranch> c_vdef_var_iden = c_vdef->getVariableIdentifierBranch();
            std::shared_ptr<Branch> c_var_name_branch = c_vdef_var_iden->getVariableNameBranch();
            if (c_var_name_branch->getValue() == var_iden_name_branch->getValue())
            {
                // Match on local scope!
                found_branch = std::dynamic_pointer_cast<VDEFBranch>(child);
                break;
            }
        }
    }

    if (found_branch == NULL)
    {
        // The root scope will never have a parent and therefore will never have scopes.
        if (lookup_scope && hasParent())
        {
            /* Ok we need to lookup the scope as we did not find the result in our own scope
             * This will act as a recursive action until either the variable is found or it is not */
            found_branch = getLocalScope()->getVariableDefinitionBranch(var_iden, true);
        }
    }

    // Are we allowed to follow a structure access?
    if (!no_follow)
    {
        if (var_iden->hasStructureAccessBranch())
        {
            if (found_branch != NULL
                    && found_branch->getType() == "STRUCT_DEF")
            {
                // We have a structure access branch so we need to keep going
                std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(found_branch);
                std::shared_ptr<BODYBranch> struct_body = struct_def_branch->getStructBody();
                std::shared_ptr<VarIdentifierBranch> next_var_iden_branch =
                        std::dynamic_pointer_cast<VarIdentifierBranch>(var_iden->getStructureAccessBranch()->getFirstChild());
                found_branch = struct_body->getVariableDefinitionBranch(next_var_iden_branch, false);
            }
        }
    }


    return found_branch;
}

std::shared_ptr<VDEFBranch> StandardScopeBranch::getVariableDefinitionBranch(std::string var_name, bool lookup_scope)
{

}
