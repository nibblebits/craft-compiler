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
 * File:   FuncCall.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 September 2016, 22:32
 * 
 * Description: 
 */

#include "FuncCallBranch.h"

FuncCallBranch::FuncCallBranch(Compiler* compiler) : CustomBranch(compiler, "FUNC_CALL", "")
{
}

FuncCallBranch::~FuncCallBranch()
{
}

void FuncCallBranch::setFuncNameBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("func_name_branch", branch);
}

void FuncCallBranch::setFuncParamsBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("func_params_branch", branch);
}

std::shared_ptr<Branch> FuncCallBranch::getFuncNameBranch()
{
    return this->getRegisteredBranchByName("func_name_branch");
}

std::shared_ptr<Branch> FuncCallBranch::getFuncParamsBranch()
{
    return this->getRegisteredBranchByName("func_params_branch");
}

void FuncCallBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<FuncCallBranch> func_call_branch_clone = std::dynamic_pointer_cast<FuncCallBranch>(cloned_branch);
    func_call_branch_clone->setFuncNameBranch(getFuncNameBranch()->clone());
    func_call_branch_clone->setFuncParamsBranch(getFuncParamsBranch()->clone());
}

std::shared_ptr<Branch> FuncCallBranch::create_clone()
{
    return std::shared_ptr<Branch>(new FuncCallBranch(getCompiler()));
}
