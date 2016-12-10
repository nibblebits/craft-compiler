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
 * File:   RootBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 08 December 2016, 15:25
 * 
 * Description: The branch for the root of the tree.
 */

#include "RootBranch.h"
#include "STRUCTBranch.h"

RootBranch::RootBranch(Compiler* compiler) : CustomBranch(compiler, "root", "")
{
}

RootBranch::~RootBranch()
{
}

std::shared_ptr<STRUCTBranch> RootBranch::getDeclaredStructureByName(std::string name)
{
    for (std::shared_ptr<Branch> child : Branch::getChildren())
    {
        if (child->getType() == "STRUCT")
        {
            std::shared_ptr<STRUCTBranch> struct_branch = std::dynamic_pointer_cast<STRUCTBranch>(child);
            std::shared_ptr<Branch> struct_name_branch = struct_branch->getStructNameBranch();
            if (struct_name_branch->getValue() == name)
                return struct_branch;
        }
    }

    return NULL;
}

void RootBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    // Clone all our children
    for(std::shared_ptr<Branch> child : cloned_branch->getChildren())
    {
        cloned_branch->addChild(child->clone());
    }
}

std::shared_ptr<Branch> RootBranch::create_clone()
{
    return std::shared_ptr<Branch>(new RootBranch(getCompiler()));
}