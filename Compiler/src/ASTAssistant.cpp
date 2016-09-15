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

ASTAssistant::ASTAssistant(Compiler* compiler) : CompilerEntity(compiler)
{
}

ASTAssistant::~ASTAssistant()
{
}

std::vector<std::shared_ptr<Branch>> ASTAssistant::findAllChildrenOfType(std::shared_ptr<Branch> root, std::string branch_type)
{
    std::vector<std::shared_ptr < Branch>> branch_list;
    std::vector<std::shared_ptr < Branch>> children = root->getChildren();

    for (std::shared_ptr<Branch> child : children)
    {
        std::vector<std::shared_ptr < Branch>> childs_child_branch_list = findAllChildrenOfType(child, branch_type);
        branch_list.insert(branch_list.end(), childs_child_branch_list.begin(), childs_child_branch_list.end());
    }

    if (root->getType() == branch_type)
    {
        // We have a match add it to the branch list
        branch_list.push_back(root);
    }

    return branch_list;
}

std::vector<std::shared_ptr<Branch>> ASTAssistant::findAllChildrenOfType(std::shared_ptr<Branch> root, std::vector<std::string> branch_types)
{
    std::vector<std::shared_ptr < Branch>> branch_list;
    std::vector<std::shared_ptr < Branch>> children = root->getChildren();
    bool found = false;

    for (std::string branch_type : branch_types)
    {
        if (root->getType() == branch_type)
        {
            // We have a match add it to the branch list
            branch_list.push_back(root);
            found = true;
        }
    }

    // We do not wish to find children of a valid child.
    if (!found)
    {
        for (std::shared_ptr<Branch> child : children)
        {
            std::vector<std::shared_ptr < Branch>> childs_child_branch_list = findAllChildrenOfType(child, branch_types);
            branch_list.insert(branch_list.end(), childs_child_branch_list.begin(), childs_child_branch_list.end());
        }
    }


    return branch_list;
}