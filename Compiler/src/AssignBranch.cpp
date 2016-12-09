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

AssignBranch::AssignBranch(Compiler* compiler) : CustomBranch(compiler, "ASSIGN", "")
{
}

AssignBranch::AssignBranch(Compiler* compiler, std::string branch_name) : CustomBranch(compiler, branch_name, "")
{

}

AssignBranch::~AssignBranch()
{
}

void AssignBranch::setVariableToAssignBranch(std::shared_ptr<Branch> var_branch)
{
    this->registerBranch("variable_to_assign_branch", var_branch);
}

void AssignBranch::setValueBranch(std::shared_ptr<Branch> value_branch)
{
    this->registerBranch("value_to_assign_branch", value_branch);
}

std::shared_ptr<Branch> AssignBranch::getVariableToAssignBranch()
{
    return this->getRegisteredBranchByName("variable_to_assign_branch");
}

std::shared_ptr<Branch> AssignBranch::getValueBranch()
{
    return this->getRegisteredBranchByName("value_to_assign_branch");
}

void AssignBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<AssignBranch> assign_branch_cloned = std::dynamic_pointer_cast<AssignBranch>(cloned_branch);
    assign_branch_cloned->setValueBranch(getValueBranch()->clone());
    assign_branch_cloned->setVariableToAssignBranch(getVariableToAssignBranch()->clone());
}

std::shared_ptr<Branch> AssignBranch::create_clone()
{
    return std::shared_ptr<Branch>(new AssignBranch(getCompiler()));
}