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

BODYBranch::BODYBranch(Compiler* compiler) : CustomBranch(compiler, "BODY", "")
{
}

BODYBranch::~BODYBranch()
{
}

std::shared_ptr<Branch> BODYBranch::findVariable(std::string variable_name)
{
    for (std::shared_ptr<Branch> branch : this->getChildren())
    {
        if (branch->getType() == "V_DEF" ||
                branch->getType() == "V_DEF_PTR" ||
                branch->getType() == "STRUCT_DEF")
        {
            std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
            std::shared_ptr<Branch> name_branch = vdef_branch->getNameBranch();
            if (name_branch->getValue() == variable_name)
                return branch;
        }
    }
    
    return NULL;
}