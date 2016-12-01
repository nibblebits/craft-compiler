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
 * File:   ASMBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 06 July 2016, 19:39
 * Rewrote on 01 December 2016, 09:42
 * 
 * Description: The branch object that represents an "ASM" branch
 */

#include "ASMBranch.h"

ASMBranch::ASMBranch(Compiler* compiler) : CustomBranch(compiler, "ASM", "")
{
}

ASMBranch::~ASMBranch()
{
}

void ASMBranch::setInstructionStartStringBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("instruction_start_str_branch", branch);
}

void ASMBranch::setInstructionArgumentsBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("instruction_args_branch", branch);
}

std::shared_ptr<Branch> ASMBranch::getInstructionStartStringBranch()
{
    return CustomBranch::getRegisteredBranchByName("instruction_start_str_branch");
}

std::shared_ptr<Branch> ASMBranch::getInstructionArgumentsBranch()
{
    return CustomBranch::getRegisteredBranchByName("instruction_args_branch");
}