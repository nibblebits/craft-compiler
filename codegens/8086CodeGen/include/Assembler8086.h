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
    OPERAND_DATA_SIZE_UNKNOWN,
    OPERAND_DATA_SIZE_BYTE,
    OPERAND_DATA_SIZE_WORD
};

enum
{
    W0_BITS_8,
    W1_BITS_16
};

enum
{
    USE_W = 0x01,
    HAS_RRR = 0x02,
    HAS_OOMMM = 0x04,
    HAS_OORRRMMM = 0x08,
    HAS_IMM_USE_LEFT = 0x10,
    HAS_IMM_USE_RIGHT = 0x20,
    HAS_REG_USE_LEFT = 0x40,
    HAS_REG_USE_RIGHT = 0x80
};

/* Eight are present, if you need more change the data type for SYNTAX_INFO to integer 
 * and adjust the code appropriately. */
enum
{
    REG8,
    REG16,
    AL,
    AX,
    MEM,
    IMM8,
    IMM16,
    ALONE
};

enum
{
    REG8_REG8 = (REG8 << 8 | REG8),
    REG16_REG16 = (REG16 << 8 | REG16),
    REG8_MEM = (REG8 << 8 | MEM),
    REG16_MEM = (REG16 << 8 | MEM),
    MEM_REG8 = (MEM << 8 | REG8),
    MEM_REG16 = (MEM << 8 | REG16),
    REG8_IMM8 = (REG8 << 8 | IMM8),
    REG16_IMM16 = (REG16 << 8 | IMM16),
    MEM_IMM8 = (MEM << 8 | IMM8),
    MEM_IMM16 = (MEM << 8 | IMM16),
    MEM_AL = (MEM << 8 | AL),
    MEM_AX = (MEM << 8 | AX),
    MEM_ALONE = (MEM << 8 | ALONE),
    AL_IMM8 = (AL << 8 | IMM8),
    AX_IMM16 = (AX << 8 | IMM16),
    IMM8_ALONE = (IMM8 << 8 | ALONE),
    IMM16_ALONE = (IMM16 << 8 | ALONE),
    REG8_ALONE = (REG8 << 8 | ALONE),
    REG16_ALONE = (REG16 << 8 | ALONE),
    AL_ALONE = (AL << 8 | ALONE),
    AX_ALONE = (AX << 8 | ALONE)
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
    MOV_REG_TO_REG_W0,
    MOV_REG_TO_REG_W1,
    MOV_IMM_TO_REG_W0,
    MOV_IMM_TO_REG_W1,
    MOV_IMM_TO_MEM_W0,
    MOV_IMM_TO_MEM_W1,
    MOV_ACC_TO_MEMOFFS_W0,
    MOV_ACC_TO_MEMOFFS_W1,
    MOV_MEMOFFS_TO_ACC_W0,
    MOV_MEMOFFS_TO_ACC_W1,
    MOV_MEM_TO_REG_W0,
    MOV_MEM_TO_REG_W1,
    MOV_REG_TO_MEM_W0,
    MOV_REG_TO_MEM_W1,
    
    ADD_REG_WITH_REG_W0,
    ADD_REG_WITH_REG_W1,
    ADD_MEM_WITH_REG_W0,
    ADD_MEM_WITH_REG_W1,
    ADD_REG_WITH_MEM_W0,
    ADD_REG_WITH_MEM_W1,
    ADD_ACC_WITH_IMM_W0,
    ADD_ACC_WITH_IMM_W1,
    ADD_REG_WITH_IMM_W0,
    ADD_REG_WITH_IMM_W1,
    ADD_MEM_WITH_IMM_W0,
    ADD_MEM_WITH_IMM_W1,
    
    SUB_REG_WITH_REG_W0,
    SUB_REG_WITH_REG_W1,
    SUB_MEM_WITH_REG_W0,
    SUB_MEM_WITH_REG_W1,
    SUB_REG_WITH_MEM_W0,
    SUB_REG_WITH_MEM_W1
    
            
            
};


typedef int INSTRUCTION_TYPE;
typedef unsigned char INSTRUCTION_INFO;
typedef unsigned short SYNTAX_INFO;
typedef unsigned short OPERAND_INFO;
typedef char OPERAND_DATA_SIZE;

struct ins_syntax_def
{
    const char* ins_name;
    INSTRUCTION_TYPE ins_type;
    SYNTAX_INFO syntax_info;
};

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

    void get_modrm_from_instruction(std::shared_ptr<InstructionBranch> ins_branch,  char* oo, char* rrr, char* mmm);
    int get_offset_from_oomod(char oo, char mmm);
    int get_instruction_size(std::shared_ptr<InstructionBranch> ins_branch);
    void register_segment(std::shared_ptr<SegmentBranch> segment_branch);
    void switch_to_segment(std::string segment_name);
    void assembler_pass_2();
    void handle_rrr(int* opcode, INSTRUCTION_INFO info, std::shared_ptr<InstructionBranch> ins_branch);
    void gen_oommm(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> ins_branch);
    void gen_oorrrmmm(std::shared_ptr<InstructionBranch> ins_branch, unsigned char def_rrr = -1);
    void gen_imm(INSTRUCTION_INFO info, std::shared_ptr<InstructionBranch> ins_branch);
    void generate_part(std::shared_ptr<Branch> branch);
    void generate_instruction(std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_reg_to_reg(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_imm_to_reg(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_imm_to_mem(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_acc_to_mem_offs(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_mem_offs_to_acc(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_mem_to_reg(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_mov_reg_to_mem(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_add_reg_with_reg(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_add_mem_with_reg(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_add_reg_with_mem(int opcode, std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_segment(std::shared_ptr<SegmentBranch> branch);

    inline char bind_modrm(char oo, char rrr, char mmm);
    inline bool has_oommm(INSTRUCTION_TYPE ins_type);
    int get_static_from_branch(std::shared_ptr<OperandBranch> branch);
    void write_modrm_offset(unsigned char oo, unsigned char mmm, std::shared_ptr<OperandBranch> branch);
    void write_abs_static8(std::shared_ptr<OperandBranch> branch);
    void write_abs_static16(std::shared_ptr<OperandBranch> branch);
    std::shared_ptr<LabelBranch> get_label_branch(std::string label_name);
    int get_label_offset(std::string label_name);
    OPERAND_INFO get_operand_info(std::shared_ptr<OperandBranch> op_branch);
    SYNTAX_INFO get_syntax_info(std::shared_ptr<InstructionBranch> instruction_branch, OPERAND_INFO* left_op = NULL, OPERAND_INFO* right_op = NULL);
    INSTRUCTION_TYPE get_instruction_type_by_name_and_syntax(std::string instruction_name, SYNTAX_INFO syntax_info);
    INSTRUCTION_TYPE get_instruction_type(std::shared_ptr<InstructionBranch> instruction_branch);
    INSTRUCTION_TYPE get_mov_ins_type(std::shared_ptr<InstructionBranch> instruction_branch);
    INSTRUCTION_TYPE get_add_ins_type(std::shared_ptr<InstructionBranch> instruction_branch);
    inline bool is_accumulator_and_not_ah(std::string _register);
    inline bool is_reg(std::string _register);
    inline char get_reg(std::string _register);
    inline bool is_mmm(std::string _register, std::string second_reg = "");
    inline char get_mmm(std::string _register, std::string second_reg = "");
    inline bool is_reg_16_bit(std::string _register);
    inline void ins_info_except();


    inline std::shared_ptr<InstructionBranch> new_ins_branch();
    void parse_part();

    std::shared_ptr<Branch> get_number_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found = false);
    std::shared_ptr<Branch> get_register_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found = false);
    std::shared_ptr<Branch> get_identifier_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found = false);
    void handle_operand_exp(std::shared_ptr<OperandBranch> operand_branch);
    void parse_operand(OPERAND_DATA_SIZE data_size = OPERAND_DATA_SIZE_UNKNOWN);
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

    INSTRUCTION_TYPE cur_ins_type;
    char mmm;
    char rrr;
    char oo;
    char op;

    int cur_offset;


};

#endif /* ASSEMBLER8086_H */

