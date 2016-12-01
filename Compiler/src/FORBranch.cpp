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
#include "BODYBranch.h"
#include "VDEFBranch.h"

FORBranch::FORBranch(Compiler* compiler) : ScopeBranch(compiler, "FOR", "")
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

void FORBranch::setBodyBranch(std::shared_ptr<BODYBranch> branch)
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

std::shared_ptr<BODYBranch> FORBranch::getBodyBranch()
{
    return std::dynamic_pointer_cast<BODYBranch>(CustomBranch::getRegisteredBranchByName("body_branch"));
}

int FORBranch::getScopeSize(bool include_subscopes)
{
    int size = 0;
    std::shared_ptr<Branch> init_branch = getInitBranch();
    if (init_branch->getType() == "V_DEF")
    {
        std::shared_ptr<VDEFBranch> vdef_init_branch = std::dynamic_pointer_cast<VDEFBranch>(init_branch);
        // The INIT branch of our "for" loop is a V_DEF so its a declaration, which means it has a size
        size = getCompiler()->getSizeOfVarDef(vdef_init_branch);
    }
    
    std::shared_ptr<BODYBranch> body_branch = getBodyBranch();
    size += body_branch->getScopeSize();
    return size;
}
