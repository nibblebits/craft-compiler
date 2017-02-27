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
 * File:   ScopeBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 01 December 2016, 17:25
 * 
 * Description: Abstract class extended by all branches who hold a scope.
 */

#include "ScopeBranch.h"
#include "VDEFBranch.h"
#include "VarIdentifierBranch.h"

ScopeBranch::ScopeBranch(Compiler* compiler, std::string name, std::string value) : CustomBranch(compiler, name, value)
{
}

ScopeBranch::~ScopeBranch()
{
}

bool ScopeBranch::invoke_scope_size_proc_if_possible(std::function<bool(std::shared_ptr<Branch> child_branch) > elem_proc, std::shared_ptr<Branch> child, bool* should_stop)
{
    if (elem_proc != NULL)
    {
        if (!elem_proc(child))
        {
            if (should_stop != NULL)
            {
                *should_stop = true;
            }
            return false;
        }
    }

    return true;
}

std::shared_ptr<VDEFBranch> ScopeBranch::getVariableDefinitionBranchFromChildren(std::shared_ptr<VarIdentifierBranch> var_iden)
{
    return getVariableDefinitionBranchFromChildren(var_iden->getVariableNameBranch()->getValue());
}

std::shared_ptr<VDEFBranch> ScopeBranch::getVariableDefinitionBranchFromChildren(std::string var_name)
{
    // Check the children to see if a V_DEF can be found with this name
    for (std::shared_ptr<Branch> child : this->getChildren())
    {
        if (child->getBranchType() == BRANCH_TYPE_VDEF)
        {
            std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(child);
            if (vdef_branch->getVariableIdentifierBranch()->getVariableNameBranch()->getValue() == var_name)
                return vdef_branch;
        }
    }
}