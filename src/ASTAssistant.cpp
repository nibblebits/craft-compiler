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
 * File:   ASTAssistant.cpp
 * Author: Daniel McCarthy
 *
 * Created on 17 June 2016, 00:50
 * 
 * Description: Provides assistance for an AST(ABSTRACT SYNTAX TREE)
 */

#include "ASTAssistant.h"

ASTAssistant::ASTAssistant() {
}

ASTAssistant::~ASTAssistant() {
}


std::vector<std::shared_ptr<Branch>> ASTAssistant::findAllChildrenOfType(std::shared_ptr<Branch> root, std::string branch_type)
{
    std::vector<std::shared_ptr<Branch>> branch_list;
    std::vector<std::shared_ptr<Branch>> children = root->getChildren();
    for (std::shared_ptr<Branch> child : children)
    {
        if (child->getType() == branch_type)
        {
            // We have a match add it to the branch list
            branch_list.push_back(child);
        }
        
        // Check the child's children
        for (std::shared_ptr<Branch> childs_child : child->getChildren())
        {
            std::vector<std::shared_ptr<Branch>> childs_child_branch_list = findAllChildrenOfType(childs_child, branch_type);
            branch_list.insert(branch_list.end(), childs_child_branch_list.begin(), childs_child_branch_list.end());
        }
    }
    
    return branch_list;
}
