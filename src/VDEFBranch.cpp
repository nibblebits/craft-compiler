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
    std::shared_ptr<Branch> branch = this->getChildren()[0];
    if (branch->getType() == "E")
    {
        /* This is a different type of variable declaration 
           likely a struct or something alike*/

        branch = branch->getChildren()[0];
    }

    return branch;
}

std::shared_ptr<Branch> VDEFBranch::getDefinitionNameBranch()
{
    return this->getChildren()[1]->getChildren()[0];
}

bool VDEFBranch::isArray()
{
    std::vector<std::shared_ptr < Branch>> children = this->getChildren();
    if (children.size() >= 3)
    {
        std::shared_ptr<Branch> third_branch = this->getChildren()[2];
        if (third_branch->getType() == "ARRAY")
        {
            return true;
        }
    }
    
    return false;
}

struct array_def VDEFBranch::getArray()
{
    struct array_def arr;
    std::vector<std::shared_ptr < Branch>> children = this->getChildren();
    if (children.size() >= 3)
    {
        // Index 2 is where the first array dimension is defined
        for (int i = 2; i < children.size(); i++)
        {
            std::shared_ptr<ArrayBranch> branch = std::dynamic_pointer_cast<ArrayBranch>(children[i]);
            if (branch->getType() == "ARRAY")
            {
                arr.sizes.push_back(std::atoi(branch->getIndexBranch()->getValue().c_str()));
            }
        }
    }

    arr.dimensions = arr.sizes.size();

    arr.t_size = arr.sizes[0];
    for (int i = 1; i < arr.dimensions; i++)
    {
        arr.t_size *= arr.sizes[i];
    }
    return arr;
}