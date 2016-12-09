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
 * File:   AddressOfBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 20 September 2016, 21:27
 * 
 * Description: 
 */

#include "AddressOfBranch.h"

AddressOfBranch::AddressOfBranch(Compiler* compiler) : CustomBranch(compiler, "ADDRESS_OF", "")
{
}

AddressOfBranch::~AddressOfBranch()
{
}

void AddressOfBranch::setVariableBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("variable_branch", branch);
}

std::shared_ptr<Branch> AddressOfBranch::getVariableBranch()
{
    return this->getRegisteredBranchByName("variable_branch");
}

void AddressOfBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<AddressOfBranch> address_of_branch_clone = std::dynamic_pointer_cast<AddressOfBranch>(cloned_branch);
    address_of_branch_clone->setVariableBranch(getVariableBranch()->clone());
}

std::shared_ptr<Branch> AddressOfBranch::create_clone()
{
    return std::shared_ptr<Branch>(new AddressOfBranch(getCompiler()));
} 