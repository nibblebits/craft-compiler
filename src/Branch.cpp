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
 * Description: This is a branch and it also holds its parent and child branches.
 */

#include "Branch.h"

Branch::Branch(std::string type, std::string value)
{
    this->type = type;
    this->value = value;
    this->parent = NULL;
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

void Branch::setParent(std::shared_ptr<Branch> branch)
{
    if (this->parent != NULL)
    {
        throw Exception("Branch::setParent(std::shared_ptr<Branch> branch): The parent has already been set");
    }
    this->parent = branch;
}

std::vector<std::shared_ptr<Branch>> Branch::getChildren()
{
    return this->children;
}

std::shared_ptr<Branch> Branch::getParent()
{
    return this->parent;
}

std::shared_ptr<Branch> Branch::lookUpTreeUntilParentTypeFound(std::string parent_type_to_find)
{
    if (this->parent == NULL)
    {
        // Not found
        return NULL;
    }

    if (this->parent->getType() == parent_type_to_find)
    {
        return this->parent;
    }
    else
    {
        return this->parent->lookUpTreeUntilParentTypeFound(parent_type_to_find);
    }
}

std::shared_ptr<Branch> Branch::lookDownTreeUntilFirstChildOfType(std::string type)
{
    for (std::shared_ptr<Branch> child : this->getChildren())
    {
        if (child->getType() == type)
        {
            return child;
        }

        child = child->lookDownTreeUntilFirstChildOfType(type);
        return child;
    }

    return NULL;
}

std::shared_ptr<Branch> Branch::lookDownTreeUntilLastChildOfType(std::string type)
{
    std::shared_ptr<Branch> branch = this->getptr();
    std::shared_ptr<Branch> last_valid_branch = NULL;
    do
    {
        branch = branch->lookDownTreeUntilFirstChildOfType(type);
        if (branch != NULL)
            last_valid_branch = branch;
    }
    while(branch != NULL);
    
    return last_valid_branch;
}

std::string Branch::getType()
{
    return this->type;
}

std::string Branch::getValue()
{
    return this->value;
}

std::shared_ptr<Branch> Branch::getptr()
{
    return shared_from_this();
}

int Branch::getBranchType()
{
    return BRANCH_TYPE_BRANCH;
}