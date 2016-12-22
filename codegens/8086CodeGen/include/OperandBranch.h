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
 * File:   OperandBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 22 December 2016, 17:26
 */

#ifndef OPERANDBRANCH_H
#define OPERANDBRANCH_H

#include "CustomBranch.h"

typedef char OPERAND_DATA_SIZE;

enum
{
    OPERAND_DATA_SIZE_BYTE,
    OPERAND_DATA_SIZE_WORD
};

class OperandBranch : public CustomBranch
{
public:
    OperandBranch(Compiler* compiler);
    virtual ~OperandBranch();

    void setOffsetBranch(std::shared_ptr<Branch> offset_branch);
    std::shared_ptr<Branch> getOffsetBranch();

    void setRegisterBranch(std::shared_ptr<Branch> register_branch);
    std::shared_ptr<Branch> getRegisterBranch();

    void setImmediateBranch(std::shared_ptr<Branch> imm_branch);
    std::shared_ptr<Branch> getImmediateBranch();

    void setMemoryAccess(bool is_memory_access);
    void setDataSize(OPERAND_DATA_SIZE size);
    bool isAccessingMemory();
    OPERAND_DATA_SIZE getDataSize();
    
    bool hasRegisterBranch();
    bool hasImmediateBranch();
    bool hasOffsetBranch();
    
    bool isOnlyRegister();
    bool isOnlyImmediate();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
    OPERAND_DATA_SIZE data_type_size;

    bool is_memory_access;
};

#endif /* OPERANDBRANCH_H */

