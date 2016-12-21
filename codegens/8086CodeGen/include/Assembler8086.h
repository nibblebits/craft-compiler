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

enum
{
    W0_BITS_8,
    W1_BITS_16
};

enum
{
    DISPLACEMENT_IF_MMM_110,
    DISPLACEMENT_8BIT_FOLLOW,
    DISPLACEMENT_16BIT_FOLLOW,
    USE_REG_NO_ADDRESSING_MODE,
};

enum
{
    MOV_REG_TO_REG_W0 = 0x88,
    MOV_REG_TO_REG_W1 = 0x89,
    MOV_IMM_TO_REG_W0 = 0x16,
    MOV_IMM_TO_REG_W1 = 0x17
};

typedef int INSTRUCTION_TYPE;

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
    void generate_part(std::shared_ptr<Branch> branch);
    void generate_instruction(std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_reg_to_reg(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_imm_to_reg(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_segment(std::shared_ptr<SegmentBranch> branch);
    char bind_modrm(char oo, char rrr, char mmm);
    INSTRUCTION_TYPE get_instruction_type(std::shared_ptr<InstructionBranch> instruction_branch);
    INSTRUCTION_TYPE get_mov_ins_type(std::shared_ptr<InstructionBranch> instruction_branch);

    inline char get_reg(std::string _register);
    inline bool is_reg_16_bit(std::string _register);


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

    std::shared_ptr<VirtualSegment> segment;
    Stream* sstream;
    std::shared_ptr<Branch> left;
    std::shared_ptr<Branch> right;
    char mmm;
    char rrr;
    char oo;
    char op;


};

#endif /* ASSEMBLER8086_H */

