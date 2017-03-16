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
 * File:   OperandBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 22 December 2016, 17:26
 */

#ifndef OPERANDBRANCH_H
#define OPERANDBRANCH_H

#include "ChildOfSegment.h"
#include "Assembler8086.h"

class InstructionBranch;

class OperandBranch : public ChildOfSegment
{
public:
    OperandBranch(Compiler* compiler, std::shared_ptr<SegmentBranch> segment_branch);
    virtual ~OperandBranch();


    void setFirstRegisterBranch(std::shared_ptr<Branch> register_branch);
    std::shared_ptr<Branch> getFirstRegisterBranch();
    
    void setSecondRegisterBranch(std::shared_ptr<Branch> register_branch);
    std::shared_ptr<Branch> getSecondRegisterBranch();


    void setNumberBranch(std::shared_ptr<Branch> imm_branch);
    std::shared_ptr<Branch> getNumberBranch();

    void setIdentifierBranch(std::shared_ptr<Branch> label_branch);
    std::shared_ptr<Branch> getIdentifierBranch();

    std::shared_ptr<InstructionBranch> getInstructionBranch();

    void setMemoryAccess(bool is_memory_access);
    void setDataSize(OPERAND_DATA_SIZE size);
    bool isAccessingMemory();
    OPERAND_DATA_SIZE getDataSize();

    bool hasRegisterBranch();
    bool hasFirstRegisterBranch();
    bool hasSecondRegisterBranch();
    bool hasNumberBranch();
    bool hasIdentifierBranch();

    bool isOnlyRegister();
    bool isOnlyImmediate();

    bool hasImmediate();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
    OPERAND_DATA_SIZE data_type_size;

    bool is_memory_access;
};

#endif /* OPERANDBRANCH_H */

