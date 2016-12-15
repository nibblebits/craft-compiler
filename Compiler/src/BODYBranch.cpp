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
 * File:   BODYBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 October 2016, 16:04
 * 
 * Description: 
 */

#include "BODYBranch.h"
#include "VDEFBranch.h"
#include "FORBranch.h"
#include "STRUCTDEFBranch.h"
#include "STRUCTBranch.h"
#include "STRUCTAccessBranch.h"
#include "RootBranch.h"

BODYBranch::BODYBranch(Compiler* compiler) : StandardScopeBranch(compiler, "BODY", "")
{
}

BODYBranch::~BODYBranch()
{
}

void BODYBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    // Clone all our children
    for (std::shared_ptr<Branch> child : Branch::getChildren())
    {
        cloned_branch->addChild(child->clone());
    }
}

std::shared_ptr<Branch> BODYBranch::create_clone()
{
    return std::shared_ptr<Branch>(new BODYBranch(getCompiler()));
}
