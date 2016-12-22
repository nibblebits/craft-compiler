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
 * File:   EBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 28 June 2016, 03:30
 * 
 * Description: The branch object for a "E" branch.
 */

#include "EBranch.h"

EBranch::EBranch(Compiler* compiler, std::string value) : CustomBranch(compiler, "E", value)
{
}

EBranch::~EBranch()
{
}

void EBranch::iterate_expressions(std::function<void(std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) > func)
{
    std::shared_ptr<Branch> left = Branch::getFirstChild();
    std::shared_ptr<Branch> right = Branch::getSecondChild();


    // Right should come before left.
    if (right->getType() == "E")
    {
        std::shared_ptr<EBranch> e_right = std::dynamic_pointer_cast<EBranch>(right);
        e_right->iterate_expressions(func);
    }

    if (left->getType() == "E")
    {
        std::shared_ptr<EBranch> e_left = std::dynamic_pointer_cast<EBranch>(left);
        e_left->iterate_expressions(func);
    }

    if (left->getType() != "E" || right->getType() != "E")
    {
        func(std::dynamic_pointer_cast<EBranch>(this->getptr()), left, right);
    }
}

void EBranch::iterate_expressions(std::function<void(std::shared_ptr<Branch> left_branch) > left_func, std::function<void(std::shared_ptr<Branch> right_branch) > right_func)
{
    std::shared_ptr<Branch> left = Branch::getFirstChild();
    std::shared_ptr<Branch> right = Branch::getSecondChild();


    // Right should come before left.
    if (right->getType() == "E")
    {
        std::shared_ptr<EBranch> e_right = std::dynamic_pointer_cast<EBranch>(right);
        e_right->iterate_expressions(left_func, right_func);
    }
    else
    {
        right_func(right);
    }

    if (left->getType() == "E")
    {
        std::shared_ptr<EBranch> e_left = std::dynamic_pointer_cast<EBranch>(left);
        e_left->iterate_expressions(left_func, right_func);
    }
    else
    {
        left_func(left);
    }

}

void EBranch::validity_check()
{
    // Lets check that this EBranch is valid
    int total_children = getChildren().size();
    if (total_children != 2)
    {
        throw Exception("void EBranch::validity_check(): expecting two children but " + std::to_string(total_children) + " were provided.");
    }
}

void EBranch::rebuild()
{
    // Ok do we need to rebuild this "EBranch" in other words has it been modified and requires to be rebuilt.
    int total_children = getChildren().size();
    if (total_children == 1)
    {
        // Ok we only have one child so we do need to rebuild here.
        // We should replace our self with our child as "E" branches are illegal with one child.
        replaceSelf(getFirstChild());
    }
    else if(total_children == 0)
    {
        // We have zero children, we should delete ourself.
        removeSelf();
    }
}

void EBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    // Clone all our children
    for (std::shared_ptr<Branch> child : this->getChildren())
    {
        cloned_branch->addChild(child->clone());
    }
}

std::shared_ptr<Branch> EBranch::create_clone()
{
    return std::shared_ptr<Branch>(new EBranch(getCompiler(), Branch::getValue()));
}