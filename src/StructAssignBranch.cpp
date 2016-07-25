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
 * File:   StructAssignBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 09 July 2016, 01:50
 * 
 * Description: The branch object for a "STRUCT_ASSIGN" branch.
 */

#include "StructAssignBranch.h"

StructAssignBranch::StructAssignBranch(Compiler* compiler) : CustomBranch(compiler, "STRUCT_ASSIGN", "")
{
}

StructAssignBranch::~StructAssignBranch()
{
}

std::shared_ptr<Branch> StructAssignBranch::getStructureNameBranch()
{
    return this->getChildren()[0]->getChildren()[0];
}

std::shared_ptr<Branch> StructAssignBranch::getAssignmentBranch()
{
    return this->getChildren()[1];
}

