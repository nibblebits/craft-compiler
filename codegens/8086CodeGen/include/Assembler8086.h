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
 * File:   Assembler8086.h
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 16:44
 */

#ifndef ASSEMBLER8086_H
#define ASSEMBLER8086_H

#include "Assembler.h"

class InstructionBranch;
class Branch;
class SegmentBranch;
class VirtualSegment;
class VirtualObjectFormat;

class Assembler8086 : public Assembler
{
public:
    Assembler8086(Compiler* compiler, std::shared_ptr<VirtualObjectFormat> object_format);
    virtual ~Assembler8086();

protected:
    virtual std::shared_ptr<Branch> parse();
    void exp_handler();
    virtual void left_exp_handler();
    virtual void right_exp_handler();
    virtual void generate();
    void generate_part(std::shared_ptr<Branch> branch, std::shared_ptr<VirtualSegment> segment);
    void generate_instruction(std::shared_ptr<InstructionBranch> instruction_branch, std::shared_ptr<VirtualSegment> segment);
    void generate_segment(std::shared_ptr<SegmentBranch> branch);

private:
    inline std::shared_ptr<InstructionBranch> new_ins_branch();
    void parse_part();
    void parse_segment();
    void parse_label();
    void parse_ins();

    inline bool is_next_segment();
    inline bool is_next_label();
    inline bool is_next_instruction();

    std::shared_ptr<Branch> root;



};

#endif /* ASSEMBLER8086_H */

