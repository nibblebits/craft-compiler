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
 * File:   AssignBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 28 June 2016, 00:01
 * 
 * Description: The branch object for a "ASSIGN" branch.
 */

#include "AssignBranch.h"
#include "ArrayBranch.h"

AssignBranch::AssignBranch(Compiler* compiler) : CustomBranch(compiler, "ASSIGN", "")
{
}

AssignBranch::~AssignBranch()
{
}

std::shared_ptr<Branch> AssignBranch::getVariableToAssignBranch()
{
    std::shared_ptr<Branch> var_root_branch = this->getChildren()[0]->getChildren()[0];
    if (this->isVariableToAssignInArray())
    {
        // Would be set to the array name e.g array[0] the name is "array"
        var_root_branch = var_root_branch->getChildren()[0];
    }
    return var_root_branch;
}

/* Returns the root branch for an array index.*/
std::shared_ptr<Branch> AssignBranch::getArrayIndexRootBranch()
{
    if (!this->isVariableToAssignInArray())
    {
        throw Exception("AssignBranch::getArrayIndexRootBranch(): Variable to assign is not in an array.");
    }

    return this->getChildren()[0]->getChildren()[1];
}

std::shared_ptr<Branch> AssignBranch::getAssignmentTypeBranch()
{
    return this->getChildren()[1];
}

std::shared_ptr<Branch> AssignBranch::getValueBranch()
{
    return this->getChildren()[2];
}

bool AssignBranch::isVariableToAssignInArray()
{
    std::shared_ptr<Branch> branch = this->getChildren()[0]->getChildren()[0];
    if (branch->getType() != "identifier")
    {
        return true;
    }

    return false;
}