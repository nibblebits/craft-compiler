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
 * File:   InstructionBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 22:31
 */

#ifndef INSTRUCTIONBRANCH_H
#define INSTRUCTIONBRANCH_H

#include "OffsetableBranch.h"

class OperandBranch;
class InstructionBranch : public OffsetableBranch
{
public:
    InstructionBranch(Compiler* compiler, std::shared_ptr<SegmentBranch> segment_branch);
    virtual ~InstructionBranch();

    void setInstructionNameBranch(std::shared_ptr<Branch> ins_name_branch);
    std::shared_ptr<Branch> getInstructionNameBranch();

    void setSize(int size);
    int getSize();
    
    void setLeftBranch(std::shared_ptr<OperandBranch> left_branch);
    void setRightBranch(std::shared_ptr<OperandBranch> right_branch);
    
    std::shared_ptr<OperandBranch> getLeftBranch();
    std::shared_ptr<OperandBranch> getRightBranch();
   
    
    bool hasLeftBranch();
    bool hasRightBranch();
    
    bool hasOnlyLeftOperandBranch();
    
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
    int size;
};

#endif /* INSTRUCTIONBRANCH_H */

