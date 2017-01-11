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
 * File:   WhileBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 06 January 2017, 06:02
 * 
 * Description: 
 */

#include "WhileBranch.h"
#include "BODYBranch.h"

WhileBranch::WhileBranch(Compiler* compiler) : CustomBranch(compiler, "WHILE", "")
{
}

WhileBranch::~WhileBranch()
{
}

void WhileBranch::setExpressionBranch(std::shared_ptr<Branch> exp_branch)
{
    CustomBranch::registerBranch("exp_branch", exp_branch);
}

void WhileBranch::setBodyBranch(std::shared_ptr<BODYBranch> body_branch)
{
    CustomBranch::registerBranch("body_branch", body_branch);
}

std::shared_ptr<Branch> WhileBranch::getExpressionBranch()
{
    return CustomBranch::getRegisteredBranchByName("exp_branch");
}

std::shared_ptr<BODYBranch> WhileBranch::getBodyBranch()
{
    return std::dynamic_pointer_cast<BODYBranch>(CustomBranch::getRegisteredBranchByName("body_branch"));
}

void WhileBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<WhileBranch> while_branch_cloned = std::dynamic_pointer_cast<WhileBranch>(cloned_branch);
    while_branch_cloned->setExpressionBranch(getExpressionBranch()->clone());
    while_branch_cloned->setBodyBranch(std::dynamic_pointer_cast<BODYBranch>(getBodyBranch()->clone()));
}

std::shared_ptr<Branch> WhileBranch::create_clone()
{
    return std::shared_ptr<Branch>(new WhileBranch(getCompiler()));
}