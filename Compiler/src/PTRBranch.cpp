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
 * File:   PtrBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 21 September 2016, 23:59
 * 
 * Description: 
 */

#include "PTRBranch.h"

PTRBranch::PTRBranch(Compiler* compiler) : CustomBranch(compiler, "PTR", "")
{
}

PTRBranch::~PTRBranch()
{
}

void PTRBranch::setVariableBranch(std::shared_ptr<Branch> var_branch)
{
    CustomBranch::registerBranch("variable_branch", var_branch);
}

std::shared_ptr<Branch> PTRBranch::getVariableBranch()
{
    return CustomBranch::getRegisteredBranchByName("variable_branch");
}