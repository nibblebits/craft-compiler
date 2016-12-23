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
    MOV_IMM_TO_REG_W1 = 0x17,
    MOV_IMM_TO_MEM_W0 = 0xc6,
    MOV_IMM_TO_MEM_W1 = 0xc7,
    MOV_ACC_TO_MEMOFFS_W0 = 0xa2,
    MOV_ACC_TO_MEMOFFS_W1 = 0xa3,
    MOV_MEMOFFS_TO_ACC_W0 = 0xa0,
    MOV_MEMOFFS_TO_ACC_W1 = 0xa1
};

typedef int INSTRUCTION_TYPE;

class OperandBranch;
class LabelBranch;

class Assembler8086 : public Assembler
{
public:
    Assembler8086(Compiler* compiler, std::shared_ptr<VirtualObjectFormat> object_format);
    virtual ~Assembler8086();

protected:
    virtual std::shared_ptr<Branch> parse();
    virtual void generate();

private:
    void exp_handler();
    virtual void left_exp_handler();
    virtual void right_exp_handler();
    void assembler_pass_1();
    void pass_1_segment(std::shared_ptr<SegmentBranch> segment_branch);
    void pass_1_part(std::shared_ptr<Branch> branch);

    void get_modrm_from_instruction(std::shared_ptr<InstructionBranch> ins_branch, char* oo, char* rrr, char* mmm);
    int get_offset_from_oomod(char oo, char mmm);
    int get_instruction_size(std::shared_ptr<InstructionBranch> ins_branch);
    void register_segment(std::shared_ptr<SegmentBranch> segment_branch);
    void switch_to_segment(std::string segment_name);
    void assembler_pass_2();
    void generate_part(std::shared_ptr<Branch> branch);
    void generate_instruction(std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_reg_to_reg(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_imm_to_reg(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_imm_to_mem(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_acc_to_mem_offs(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_mem_offs_to_acc(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_segment(std::shared_ptr<SegmentBranch> branch);
    char bind_modrm(char oo, char rrr, char mmm);
    int get_static_from_branch(std::shared_ptr<OperandBranch> branch);
    void write_abs_static8(std::shared_ptr<OperandBranch> branch);
    void write_abs_static16(std::shared_ptr<OperandBranch> branch);
    std::shared_ptr<LabelBranch> get_label_branch(std::string label_name);
    int get_label_offset(std::string label_name);
    INSTRUCTION_TYPE get_instruction_type(std::shared_ptr<InstructionBranch> instruction_branch);
    INSTRUCTION_TYPE get_mov_ins_type(std::shared_ptr<InstructionBranch> instruction_branch);

    inline bool is_accumulator_and_not_ah(std::string _register);
    inline bool is_reg(std::string _register);
    inline char get_reg(std::string _register);
    inline bool is_reg_16_bit(std::string _register);


    inline std::shared_ptr<InstructionBranch> new_ins_branch();
    void parse_part();
    
    std::shared_ptr<Branch> get_number_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found = false);
    std::shared_ptr<Branch> get_register_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found = false);
    std::shared_ptr<Branch> get_identifier_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found = false);
    void handle_operand_exp(std::shared_ptr<OperandBranch> operand_branch);
    void parse_operand();
    void parse_segment();
    void parse_label();
    void parse_ins();

    inline bool is_next_valid_operand();
    inline bool is_next_segment();
    inline bool is_next_label();
    inline bool is_next_instruction();

    std::shared_ptr<Branch> root;

    std::shared_ptr<VirtualSegment> segment;
    std::shared_ptr<SegmentBranch> segment_branch;
    std::vector<std::shared_ptr<VirtualSegment>> segments;
    Stream* sstream;
    std::shared_ptr<OperandBranch> left;
    std::shared_ptr<Branch> left_reg;
    std::shared_ptr<OperandBranch> right;
    std::shared_ptr<Branch> right_reg;


    char mmm;
    char rrr;
    char oo;
    char op;

    int cur_offset;


};

#endif /* ASSEMBLER8086_H */

