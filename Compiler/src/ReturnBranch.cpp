/*
    Craft compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   ReturnBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 21 January 2017, 03:07
 * 
 * Description: 
 */

#include "ReturnBranch.h"
#include "CustomBranch.h"

ReturnBranch::ReturnBranch(Compiler* compiler) : CustomBranch(compiler, "RETURN", "")
{
}

ReturnBranch::~ReturnBranch()
{
}

void ReturnBranch::setExpressionBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("exp_branch", branch);
}

std::shared_ptr<Branch> ReturnBranch::getExpressionBranch()
{
    return CustomBranch::getRegisteredBranchByName("exp_branch");
}

bool ReturnBranch::hasExpressionBranch()
{
    return CustomBranch::isBranchRegistered("exp_branch");
}

void ReturnBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<ReturnBranch> return_branch_cloned = std::dynamic_pointer_cast<ReturnBranch>(cloned_branch);
    if (return_branch_cloned->hasExpressionBranch())
    {
        return_branch_cloned->setExpressionBranch(return_branch_cloned->getExpressionBranch()->clone());
    }
}

std::shared_ptr<Branch> ReturnBranch::create_clone()
{
    return std::shared_ptr<Branch>(new ReturnBranch(getCompiler()));
}