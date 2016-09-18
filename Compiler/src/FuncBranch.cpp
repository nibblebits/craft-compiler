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
 * File:   FuncBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 25 June 2016, 01:30
 * 
 * Description: The branch object for a "FUNC" branch.
 */

#include "FuncBranch.h"

FuncBranch::FuncBranch(Compiler* compiler) : CustomBranch(compiler, "FUNC", "")
{
}

FuncBranch::~FuncBranch()
{
}

    void FuncBranch::setReturnTypeBranch(std::shared_ptr<Branch> returnTypeBranch)
    {
        this->registerBranch("func_return_type_branch", returnTypeBranch);
    }
void FuncBranch::setNameBranch(std::shared_ptr<Branch> nameBranch)
{
    this->registerBranch("func_name_branch", nameBranch);
}

void FuncBranch::setArgumentsBranch(std::shared_ptr<Branch> argumentsBranch)
{
    this->registerBranch("func_arguments_branch", argumentsBranch);
}

void FuncBranch::setBodyBranch(std::shared_ptr<Branch> bodyBranch)
{
    this->registerBranch("func_body_branch", bodyBranch);
}

std::shared_ptr<Branch> FuncBranch::getReturnTypeBranch()
{
    return this->getRegisteredBranchByName("func_return_type_branch");
}

std::shared_ptr<Branch> FuncBranch::getNameBranch()
{
    return this->getRegisteredBranchByName("func_name_branch");
}

std::shared_ptr<Branch> FuncBranch::getArgumentsBranch()
{
    return this->getRegisteredBranchByName("func_arguments_branch");
}

std::shared_ptr<Branch> FuncBranch::getBodyBranch()
{
    return this->getRegisteredBranchByName("func_body_branch");
}
