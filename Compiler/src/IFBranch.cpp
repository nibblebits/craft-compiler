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

IFBranch::IFBranch(Compiler* compiler) : CustomBranch(compiler, "IF", "")
{
}

IFBranch::~IFBranch()
{
}

void IFBranch::setExpressionBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("exp_branch", branch);
}

void IFBranch::setBodyBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("body_branch", branch);
}

std::shared_ptr<Branch> IFBranch::getExpressionBranch()
{
    return this->getRegisteredBranchByName("exp_branch");
}

std::shared_ptr<Branch> IFBranch::getBodyBranch()
{
    return this->getRegisteredBranchByName("body_branch");
}

