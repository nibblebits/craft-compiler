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
 * File:   Tree.cpp
 * Author: Daniel McCarthy
 *
 * Created on 29 May 2016, 20:35
 * 
 * Description: Holds helper methods and also holds children of an AST(Abstract Syntax Tree)
 */

#include "Tree.h"
#include "STRUCTBranch.h"

Tree::Tree()
{
    this->root = NULL;
}

Tree::~Tree()
{
}

std::shared_ptr<STRUCTBranch> Tree::getGlobalStructureByName(std::string name)
{
    std::vector<std::shared_ptr<Branch>> root_children = this->root->getChildren();
    for (std::shared_ptr<Branch> child : root_children)
    {
        if (child->getType() == "STRUCT")
        {
            std::shared_ptr<STRUCTBranch> struct_branch = std::dynamic_pointer_cast<STRUCTBranch>(child);
            std::shared_ptr<Branch> struct_name_branch = struct_branch->getStructNameBranch();
            if (struct_name_branch->getValue() == name)
            {
                return struct_branch;
            }
        }
    }
    
    return NULL;
}
