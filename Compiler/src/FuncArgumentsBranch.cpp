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
 * File:   FuncArgumentsBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 17 December 2016, 13:59
 * 
 * Description: 
 */

#include "FuncArgumentsBranch.h"

FuncArgumentsBranch::FuncArgumentsBranch(Compiler* compiler) : StandardScopeBranch(compiler, "FUNC_ARGUMENTS", "")
{
}

FuncArgumentsBranch::~FuncArgumentsBranch()
{
}

void FuncArgumentsBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    iterate_children([&](std::shared_ptr<Branch> child)
    {
        cloned_branch->addChild(child->clone());
    });
}

std::shared_ptr<Branch> FuncArgumentsBranch::create_clone()
{
    return std::shared_ptr<Branch>(new FuncArgumentsBranch(getCompiler()));
}