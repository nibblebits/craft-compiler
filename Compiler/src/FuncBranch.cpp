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
 * File:   FuncBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 25 June 2016, 01:30
 * 
 * Description: The branch object for a "FUNC" branch.
 */

#include "FuncBranch.h"
#include "BODYBranch.h"

FuncBranch::FuncBranch(Compiler* compiler) : FuncDefBranch(compiler, "FUNC", "")
{
}

FuncBranch::~FuncBranch()
{
}

void FuncBranch::setBodyBranch(std::shared_ptr<BODYBranch> bodyBranch)
{
    this->registerBranch("func_body_branch", bodyBranch);
}


std::shared_ptr<BODYBranch> FuncBranch::getBodyBranch()
{
    return std::dynamic_pointer_cast<BODYBranch>(this->getRegisteredBranchByName("func_body_branch"));
}

void FuncBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    FuncDefBranch::imp_clone(cloned_branch);
    
    std::shared_ptr<FuncBranch> func_branch_cloned = std::dynamic_pointer_cast<FuncBranch>(cloned_branch);
    func_branch_cloned->setBodyBranch(std::dynamic_pointer_cast<BODYBranch>(getBodyBranch()->clone()));
}
std::shared_ptr<Branch> FuncBranch::create_clone()
{
    return std::shared_ptr<Branch>(new FuncBranch(getCompiler()));
}
