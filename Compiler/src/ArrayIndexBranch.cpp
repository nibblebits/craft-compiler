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
 * File:   ArrayIndex.cpp
 * Author: Daniel McCarthy
 *
 * Created on 04 November 2016, 02:27
 * 
 * Description: 
 */

#include "ArrayIndexBranch.h"

ArrayIndexBranch::ArrayIndexBranch(Compiler* compiler) : CustomBranch(compiler, "ARRAY_INDEX", "")
{
}

ArrayIndexBranch::~ArrayIndexBranch()
{
}

void ArrayIndexBranch::setValueBranch(std::shared_ptr<Branch> value_branch)
{
    CustomBranch::registerBranch("value_branch", value_branch);
}

void ArrayIndexBranch::setNextArrayIndexBranch(std::shared_ptr<Branch> next_array_index_branch)
{
    CustomBranch::registerBranch("next_array_index_branch", next_array_index_branch);
}

std::shared_ptr<Branch> ArrayIndexBranch::getValueBranch()
{
    return CustomBranch::getRegisteredBranchByName("value_branch");
}

std::shared_ptr<Branch> ArrayIndexBranch::getNextArrayIndexBranch()
{
    return CustomBranch::getRegisteredBranchByName("next_array_index_branch");
}

bool ArrayIndexBranch::hasNextArrayIndexBranch()
{
    std::shared_ptr<Branch> next_array_index_branch = getNextArrayIndexBranch();
    return next_array_index_branch != NULL;
}

bool ArrayIndexBranch::hasParentArrayIndexBranch()
{
    std::shared_ptr<Branch> parent = Branch::getParent();
    return parent->getType() == "ARRAY_INDEX";
}

std::shared_ptr<Branch> ArrayIndexBranch::getDeepestArrayIndexBranch()
{
    std::shared_ptr<ArrayIndexBranch> current_branch = std::dynamic_pointer_cast<ArrayIndexBranch>(this->getptr());
    while (current_branch->hasNextArrayIndexBranch())
    {
        current_branch = std::dynamic_pointer_cast<ArrayIndexBranch>(current_branch->getNextArrayIndexBranch());
    }

    return current_branch;
}

void ArrayIndexBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<ArrayIndexBranch> array_index_cloned_branch = std::dynamic_pointer_cast<ArrayIndexBranch>(cloned_branch);
    array_index_cloned_branch->setValueBranch(getValueBranch()->clone());
    array_index_cloned_branch->setNextArrayIndexBranch(getNextArrayIndexBranch()->clone());
}

std::shared_ptr<Branch> ArrayIndexBranch::create_clone()
{
    return std::shared_ptr<Branch>(new ArrayIndexBranch(getCompiler()));
}
