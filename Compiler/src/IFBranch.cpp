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
 * File:   IFBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 02 October 2016, 03:20
 * 
 * Description: 
 */

#include "IFBranch.h"
#include "BODYBranch.h"

IFBranch::IFBranch(Compiler* compiler) : CustomBranch(compiler, "IF", "")
{
}

IFBranch::~IFBranch()
{
}

void IFBranch::setExpressionBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("exp_branch", branch);
}

void IFBranch::setBodyBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("body_branch", branch);
}

void IFBranch::setElseBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("else_branch", branch);
}

void IFBranch::setElseIfBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("else_if_branch", branch);
}

std::shared_ptr<Branch> IFBranch::getExpressionBranch()
{
    return CustomBranch::getRegisteredBranchByName("exp_branch");
}

std::shared_ptr<BODYBranch> IFBranch::getBodyBranch()
{
    return std::dynamic_pointer_cast<BODYBranch>(CustomBranch::getRegisteredBranchByName("body_branch"));
}

std::shared_ptr<Branch> IFBranch::getElseBranch()
{
    return CustomBranch::getRegisteredBranchByName("else_branch");
}

std::shared_ptr<Branch> IFBranch::getElseIfBranch()
{
    return CustomBranch::getRegisteredBranchByName("else_if_branch");
}

bool IFBranch::hasElseBranch()
{
    if (CustomBranch::isBranchRegistered("else_branch"))
    {
        return true;
    }

    return false;
}

bool IFBranch::hasElseIfBranch()
{
    if (CustomBranch::isBranchRegistered("else_if_branch"))
    {
        return true;
    }

    return false;
}
