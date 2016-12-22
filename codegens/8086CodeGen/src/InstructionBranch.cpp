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
 * File:   InstructionBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 22:31
 * 
 * Description: 
 */

#include "InstructionBranch.h"
#include "OperandBranch.h"

InstructionBranch::InstructionBranch(Compiler* compiler) : CustomBranch(compiler, "INSTRUCTION", "")
{
}

InstructionBranch::~InstructionBranch()
{
}

void InstructionBranch::setInstructionNameBranch(std::shared_ptr<Branch> ins_name_branch)
{
    CustomBranch::registerBranch("instruction_name_branch", ins_name_branch);
}

std::shared_ptr<Branch> InstructionBranch::getInstructionNameBranch()
{
    return CustomBranch::getRegisteredBranchByName("instruction_name_branch");
}

void InstructionBranch::setLeftBranch(std::shared_ptr<OperandBranch> left_branch)
{
    CustomBranch::registerBranch("left_branch", left_branch);
}

void InstructionBranch::setRightBranch(std::shared_ptr<OperandBranch> right_branch)
{
    CustomBranch::registerBranch("right_branch", right_branch);
}

std::shared_ptr<OperandBranch> InstructionBranch::getLeftBranch()
{
    return std::dynamic_pointer_cast<OperandBranch>(CustomBranch::getRegisteredBranchByName("left_branch"));
}

std::shared_ptr<OperandBranch> InstructionBranch::getRightBranch()
{
    return std::dynamic_pointer_cast<OperandBranch>(CustomBranch::getRegisteredBranchByName("right_branch"));
}

void InstructionBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<InstructionBranch> ins_branch = std::dynamic_pointer_cast<InstructionBranch>(cloned_branch);
    ins_branch->setInstructionNameBranch(getInstructionNameBranch()->clone());
    ins_branch->setLeftBranch(std::dynamic_pointer_cast<OperandBranch>(getLeftBranch()->clone()));
    ins_branch->setRightBranch(std::dynamic_pointer_cast<OperandBranch>(getRightBranch()->clone()));
}

std::shared_ptr<Branch> InstructionBranch::create_clone()
{
    return std::shared_ptr<Branch>(new InstructionBranch(getCompiler()));
}