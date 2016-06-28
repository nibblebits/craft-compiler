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
 * File:   AssignBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 28 June 2016, 00:01
 * 
 * Description: The branch object for a "ASSIGN" branch.
 */

#include "AssignBranch.h"

AssignBranch::AssignBranch(Compiler* compiler) : CustomBranch(compiler, "ASSIGN", "")
{
}

AssignBranch::~AssignBranch()
{
}

std::shared_ptr<Branch> AssignBranch::getVariableToAssignBranch()
{
    return this->getChildren()[0]->getChildren()[0];
}

std::shared_ptr<Branch> AssignBranch::getAssignmentTypeBranch()
{
    return this->getChildren()[1];
}

std::shared_ptr<Branch> AssignBranch::getValueBranch()
{
    return this->getChildren()[2]->getChildren()[0];
}