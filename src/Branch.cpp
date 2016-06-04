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
 * File:   Branch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 29 May 2016, 20:41
 * 
 * Description: Holds child branches
 */

#include "Branch.h"

Branch::Branch(std::string type, std::string value)
{
    this->type = type;
    this->value = value;
    this->excluded_from_tree = false;
}

Branch::~Branch()
{

}

void Branch::addChild(std::shared_ptr<Branch> branch)
{
    this->children.push_back(branch);
}

void Branch::exclude(bool excluded)
{
    this->excluded_from_tree = excluded;
}

bool Branch::excluded()
{
    return this->excluded_from_tree;
}

std::vector<std::shared_ptr<Branch>> Branch::getChildren()
{
    return this->children;
}

std::string Branch::getType()
{
    return this->type;
}

std::string Branch::getValue()
{
    return this->value;
}

int Branch::getBranchType()
{
    return BRANCH_TYPE_BRANCH;
}