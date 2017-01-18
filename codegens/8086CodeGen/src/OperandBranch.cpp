/*
    Craft Compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   OperandBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 22 December 2016, 17:26
 * 
 * Description: 
 */

#include "OperandBranch.h"

OperandBranch::OperandBranch(Compiler* compiler, std::shared_ptr<SegmentBranch> segment_branch) : ChildOfSegment(compiler, segment_branch, "OPERAND", "")
{
    this->is_memory_access = false;
}

OperandBranch::~OperandBranch()
{
}

void OperandBranch::setRegisterBranch(std::shared_ptr<Branch> register_branch)
{
    CustomBranch::registerBranch("register_branch", register_branch);
}

std::shared_ptr<Branch> OperandBranch::getRegisterBranch()
{
    return CustomBranch::getRegisteredBranchByName("register_branch");
}

void OperandBranch::setNumberBranch(std::shared_ptr<Branch> imm_branch)
{
    CustomBranch::registerBranch("number_branch", imm_branch);
}

std::shared_ptr<Branch> OperandBranch::getNumberBranch()
{
    return CustomBranch::getRegisteredBranchByName("number_branch");
}

void OperandBranch::setIdentifierBranch(std::shared_ptr<Branch> label_branch)
{
    CustomBranch::registerBranch("label_branch", label_branch);
}

std::shared_ptr<Branch> OperandBranch::getIdentifierBranch()
{
    return CustomBranch::getRegisteredBranchByName("label_branch");
}

void OperandBranch::setMemoryAccess(bool is_memory_access)
{
    this->is_memory_access = is_memory_access;
}

void OperandBranch::setDataSize(OPERAND_DATA_SIZE size)
{
    this->data_type_size = size;
}

bool OperandBranch::hasRegisterBranch()
{
    return CustomBranch::isBranchRegistered("register_branch");
}

bool OperandBranch::hasNumberBranch()
{
    return CustomBranch::isBranchRegistered("number_branch");
}

bool OperandBranch::hasIdentifierBranch()
{
    return CustomBranch::isBranchRegistered("label_branch");
}

bool OperandBranch::isOnlyRegister()
{
    return hasRegisterBranch()
            && !isAccessingMemory()
            && !hasNumberBranch()
            && !hasIdentifierBranch();
}

bool OperandBranch::isOnlyImmediate()
{
    return ((hasNumberBranch() || hasIdentifierBranch())
            && !isAccessingMemory());
}

bool OperandBranch::hasImmediate()
{
    return (hasNumberBranch() || hasIdentifierBranch());
}

bool OperandBranch::isAccessingMemory()
{
    return this->is_memory_access;
}

OPERAND_DATA_SIZE OperandBranch::getDataSize()
{
    return this->data_type_size;
}

void OperandBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<OperandBranch> op_branch = std::dynamic_pointer_cast<OperandBranch>(cloned_branch);
    op_branch->setRegisterBranch(getRegisterBranch()->clone());
    op_branch->setNumberBranch(getNumberBranch()->clone());
    op_branch->setIdentifierBranch(getIdentifierBranch()->clone());
    op_branch->setMemoryAccess(isAccessingMemory());
}

std::shared_ptr<Branch> OperandBranch::create_clone()
{
    return std::shared_ptr<Branch>(new OperandBranch(getCompiler(), getSegmentBranch()));
}