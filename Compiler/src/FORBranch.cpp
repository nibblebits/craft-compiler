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
 * File:   FORBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 20 October 2016, 04:19
 * 
 * Description: 
 */

#include "FORBranch.h"

FORBranch::FORBranch(Compiler* compiler) : CustomBranch(compiler, "FOR", "")
{
}

FORBranch::~FORBranch()
{
}

void FORBranch::setInitBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("init_branch", branch);
}

void FORBranch::setCondBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("cond_branch", branch);
}

void FORBranch::setLoopBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("loop_branch", branch);
}

void FORBranch::setBodyBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("body_branch", branch);
}

std::shared_ptr<Branch> FORBranch::getInitBranch()
{
    return CustomBranch::getRegisteredBranchByName("init_branch");
}

std::shared_ptr<Branch> FORBranch::getCondBranch()
{
    return CustomBranch::getRegisteredBranchByName("cond_branch");
}

std::shared_ptr<Branch> FORBranch::getLoopBranch()
{
    return CustomBranch::getRegisteredBranchByName("loop_branch");
}

std::shared_ptr<Branch> FORBranch::getBodyBranch()
{
    return CustomBranch::getRegisteredBranchByName("body_branch");
}
