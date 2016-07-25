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
 * File:   ArrayBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 11 July 2016, 05:00
 * 
 * Description: 
 */

#include "ArrayBranch.h"

ArrayBranch::ArrayBranch(Compiler* compiler) : CustomBranch(compiler, "ARRAY", "")
{
}

ArrayBranch::~ArrayBranch()
{
}

bool ArrayBranch::isBranchAnArrayHolder()
{
    std::shared_ptr<Branch> branch = this->getChildren()[0];
    if (branch->getValue() == "ARRAY")
    {
        return true;
    }

    return false;
}

std::shared_ptr<Branch> ArrayBranch::getIndexBranch()
{
    std::shared_ptr<Branch> branch = this->getChildren()[0];
    if (this->isBranchAnArrayHolder())
    {
        throw Exception("Cannot get index for an array holder");
    }

    return this->getChildren()[0]->getChildren()[0];
}

int ArrayBranch::getIndexBranchNumber()
{
    std::shared_ptr<Branch> branch = this->getIndexBranch();
    if (branch->getType() != "number")
    {
        return -1;
    }

    return std::stoi(branch->getValue());
}