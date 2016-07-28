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
 * File:   VDEFBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 25 June 2016, 02:31
 * 
 * Description: The branch object for a "VDEF" branch.
 */

#include "VDEFBranch.h"
#include "ArrayBranch.h"


VDEFBranch::VDEFBranch(Compiler* compiler) : CustomBranch(compiler, "V_DEF", "")
{
}

VDEFBranch::~VDEFBranch()
{
}

std::shared_ptr<Branch> VDEFBranch::getDefinitionTypeBranch()
{
    return this->getFirstChild();
}

std::shared_ptr<Branch> VDEFBranch::getDefinitionNameBranch()
{
    if (isArray())
    {
        // Return the array name.
        return this->getSecondChild()->getFirstChild();
    }
    return this->getSecondChild();
}

std::shared_ptr<Branch> VDEFBranch::getDefinitionArrayBranch()
{
    if (!isArray())
    {
        throw Exception("std::shared_ptr<Branch> VDEFBranch::getDefinitionArrayBranch(): The V_DEF branch is not a variable declaration for an array");
    }

    return this->getSecondChild();
}

bool VDEFBranch::isArray()
{
    return this->getSecondChild()->getType() == "ARRAY";
}

struct array_def VDEFBranch::getArray()
{
    if (!this->isArray())
    {
        throw Exception("struct array_def VDEFBranch::getArray(): The V_DEF branch is not a variable declaration for an array");
    }
    /*
    struct array_def arr;
    std::shared_ptr<ArrayBranch> array_branch = this->getDefinitionArrayBranch();
    arr.dimensions = arr.sizes.size();

    arr.t_size = arr.sizes[0];
    for (int i = 1; i < arr.dimensions; i++)
    {
        arr.t_size *= arr.sizes[i];
    }
    return arr;
     */
}
  