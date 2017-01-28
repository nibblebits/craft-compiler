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
 * File:   Assembler8086.h
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 16:44
 */

#ifndef ASSEMBLER8086_H
#define ASSEMBLER8086_H

#include "definitions.h"
#include "Assembler.h"

class InstructionBranch;
class Branch;
class SegmentBranch;
class OperandBranch;
class LabelBranch;
class DataBranch;
class OffsetableBranch;

typedef int INSTRUCTION_TYPE;
typedef unsigned short INSTRUCTION_INFO;
typedef unsigned int SYNTAX_INFO;
typedef unsigned short OPERAND_INFO;
typedef char CONDITION_CODE;
typedef char OPERAND_DATA_SIZE;
#define OPERAND_BIT_SIZE sizeof(OPERAND_INFO) * 8

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

// Condition codes

enum
{
    OVERFLOW,
    NOT_OVERFLOW,
    CARRY_BELOW_NOT_ABOVE_NOR_EQUAL,
    NOT_CARRY_ABOVE_OR_EQUAL_NOT_BELOW,
    EQUAL_ZERO,
    NOT_EQUAL_NOT_ZERO,
    BELOW_OR_EQUAL_NOT_ABOVE,
    ABOVE_NOT_BELOW_NOR_EQUAL,
    SIGN_NEGATIVE,
    NOT_SIGN,
    PARITY_PARITY_EVEN,
    NOT_PARITY_PARITY_ODD,
    LESS_NOT_GREATER_NOR_EQUAL,
    GREATER_OR_EQUAL_NOT_LESS,
    LESS_OR_EQUAL_NOT_GREATER,
    GREATER_NOT_LESS_NOR_EQUAL
};

enum
{
    NO_PROPERTIES = 0x00,
    USE_W = 0x01,
    HAS_RRR = 0x02,
    HAS_OOMMM = 0x04,
    HAS_OORRRMMM = 0x08,
    HAS_IMM_USE_LEFT = 0x10,
    HAS_IMM_USE_RIGHT = 0x20,
    HAS_REG_USE_LEFT = 0x40,
    HAS_REG_USE_RIGHT = 0x80,
    SHORT_POSSIBLE = 0x100,
    NEAR_POSSIBLE = 0x200,
    USE_CONDITION_CODE = 0x400
};

enum
{
    REG8,
    REG16,
    AL,
    AX,
    MEM8,
    MEM16,
    IMM8,
    IMM16,
    ALONE
};

enum
{
    REG8_REG8 = (REG8 << OPERAND_BIT_SIZE | REG8),
    REG16_REG16 = (REG16 << OPERAND_BIT_SIZE | REG16),
    REG8_MEM16 = (REG8 << OPERAND_BIT_SIZE | MEM16),
    REG16_MEM16 = (REG16 << OPERAND_BIT_SIZE | MEM16),
    MEM16_REG8 = (MEM16 << OPERAND_BIT_SIZE | REG8),
    MEM16_REG16 = (MEM16 << OPERAND_BIT_SIZE | REG16),
    REG8_IMM8 = (REG8 << OPERAND_BIT_SIZE | IMM8),
    REG16_IMM16 = (REG16 << OPERAND_BIT_SIZE | IMM16),
    REG16_IMM8 = (REG16 << OPERAND_BIT_SIZE | IMM8),
    MEM16_IMM8 = (MEM16 << OPERAND_BIT_SIZE | IMM8),
    MEM16_IMM16 = (MEM16 << OPERAND_BIT_SIZE | IMM16),
    MEM16_AL = (MEM16 << OPERAND_BIT_SIZE | AL),
    MEM16_AX = (MEM16 << OPERAND_BIT_SIZE | AX),
    MEM16_ALONE = (MEM16 << OPERAND_BIT_SIZE | ALONE),
    MEM8_ALONE = (MEM8 << OPERAND_BIT_SIZE | ALONE),
    AL_IMM8 = (AL << OPERAND_BIT_SIZE | IMM8),
    AX_IMM16 = (AX << OPERAND_BIT_SIZE | IMM16),
    IMM8_ALONE = (IMM8 << OPERAND_BIT_SIZE | ALONE),
    IMM16_ALONE = (IMM16 << OPERAND_BIT_SIZE | ALONE),
    REG8_ALONE = (REG8 << OPERAND_BIT_SIZE | ALONE),
    REG16_ALONE = (REG16 << OPERAND_BIT_SIZE | ALONE),
    AL_ALONE = (AL << OPERAND_BIT_SIZE | ALONE),
    AX_ALONE = (AX << OPERAND_BIT_SIZE | ALONE),
    ALONE_ALONE = (ALONE << OPERAND_BIT_SIZE | ALONE)
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
    SUB_REG_WITH_MEM_W1,
    SUB_ACC_WITH_IMM_W0,
    SUB_ACC_WITH_IMM_W1,
    SUB_REG_WITH_IMM_W0,
    SUB_REG_WITH_IMM_W1,
    SUB_MEM_WITH_IMM_W0,
    SUB_MEM_WITH_IMM_W1,

    MUL_WITH_REG_W0,
    MUL_WITH_REG_W1,
    MUL_WITH_MEM_W0,
    MUL_WITH_MEM_W1,

    DIV_WITH_REG_W0,
    DIV_WITH_REG_W1,
    DIV_WITH_MEM_W0,
    DIV_WITH_MEM_W1,

    JMP_SHORT,
    JMP_NEAR,

    CALL_NEAR,

    // Only the jump instructions that the code generator uses are present
    JE_SHORT,
    JNE_SHORT,
    JG_SHORT,
    JA_SHORT,
    JLE_SHORT,
    JBE_SHORT,
    JL_SHORT,
    JB_SHORT,
    JGE_SHORT,
    JAE_SHORT,

    PUSH_REG16,

    POP_REG16,

    RET,

    XOR_REG_WITH_REG_W0,
    XOR_REG_WITH_REG_W1,
    XOR_MEM_WITH_REG_W0,
    XOR_MEM_WITH_REG_W1,
    XOR_REG_WITH_MEM_W0,
    XOR_REG_WITH_MEM_W1,
    XOR_ACC_WITH_IMM_W0,
    XOR_ACC_WITH_IMM_W1,
    XOR_REG_WITH_IMM_W0,
    XOR_REG_WITH_IMM_W1
};

struct ins_syntax_def
{
    const char* ins_name;
    INSTRUCTION_TYPE ins_type;
    SYNTAX_INFO syntax_info;
};

struct condition_code_instruction
{
    const char* ins_name;
    CONDITION_CODE code;
};

class MustFitTable;
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
    virtual void push_branch(std::shared_ptr<Branch> branch);
    
    std::shared_ptr<MustFitTable> get_must_fit_table_for_label(std::string label_name);
    void assembler_pass_1();
    void pass_1_segment(std::shared_ptr<SegmentBranch> segment_branch);
    void pass_1_part(std::shared_ptr<Branch> branch);

    void assembler_pass_2();
    void pass_2_segment(std::shared_ptr<SegmentBranch> segment_branch);
    void pass_2_part(std::shared_ptr<Branch> branch);
    void handle_mustfits_for_label_branch(std::shared_ptr<LabelBranch> label_branch);
    
    void get_modrm_from_instruction(std::shared_ptr<InstructionBranch> ins_branch, char* oo, char* rrr, char* mmm);
    int get_offset_from_oomod(char oo, char mmm);
    int get_instruction_size(std::shared_ptr<InstructionBranch> ins_branch);
    void register_segment(std::shared_ptr<SegmentBranch> segment_branch);
    void switch_to_segment(std::string segment_name);
    void assembler_pass_3();
    void handle_rrr(int* opcode, INSTRUCTION_INFO info, std::shared_ptr<InstructionBranch> ins_branch);
    void handle_condition_code(int* opcode, std::shared_ptr<InstructionBranch> ins_branch);
    void gen_oommm(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> ins_branch);
    void gen_oorrrmmm(std::shared_ptr<InstructionBranch> ins_branch, unsigned char def_rrr = -1);
    void gen_imm(INSTRUCTION_INFO info, std::shared_ptr<InstructionBranch> ins_branch);
    void generate_part(std::shared_ptr<Branch> branch);
    void generate_instruction(std::shared_ptr<InstructionBranch> instruction_branch);
    void generate_data(std::shared_ptr<DataBranch> data_branch);
    void generate_segment(std::shared_ptr<SegmentBranch> branch);

    inline char bind_modrm(char oo, char rrr, char mmm);
    inline bool has_oommm(INSTRUCTION_TYPE ins_type);
    CONDITION_CODE get_condition_code_for_instruction(std::string instruction_name);
    int get_static_from_branch(std::shared_ptr<OperandBranch> branch, bool short_or_near_possible = false, std::shared_ptr<InstructionBranch> ins_branch = NULL);
    std::shared_ptr<VirtualSegment> get_virtual_segment_for_label(std::string label_name);
    void register_fixup_if_required(int offset, FIXUP_LENGTH length, std::shared_ptr<OperandBranch> branch);
    void write_modrm_offset(unsigned char oo, unsigned char mmm, std::shared_ptr<OperandBranch> branch);
    unsigned char write_abs_static8(std::shared_ptr<OperandBranch> branch, bool short_possible = false, std::shared_ptr<InstructionBranch> ins_branch = NULL);
    unsigned short write_abs_static16(std::shared_ptr<OperandBranch> branch, bool near_possible = false, std::shared_ptr<InstructionBranch> ins_branch = NULL);
    std::shared_ptr<LabelBranch> get_label_branch(std::string label_name);
    int get_label_offset(std::string label_name);
    OPERAND_INFO get_operand_info(std::shared_ptr<OperandBranch> op_branch);
    SYNTAX_INFO get_syntax_info(std::shared_ptr<InstructionBranch> instruction_branch, OPERAND_INFO* left_op = NULL, OPERAND_INFO* right_op = NULL);
    INSTRUCTION_TYPE get_instruction_type_by_name_and_syntax(std::string instruction_name, SYNTAX_INFO syntax_info);
    INSTRUCTION_TYPE get_instruction_type(std::shared_ptr<InstructionBranch> instruction_branch);
    INSTRUCTION_TYPE get_mov_ins_type(std::shared_ptr<InstructionBranch> instruction_branch);
    INSTRUCTION_TYPE get_add_ins_type(std::shared_ptr<InstructionBranch> instruction_branch);

    OPERAND_DATA_SIZE get_data_size_for_reg(std::string reg);
    OPERAND_DATA_SIZE get_operand_data_size_for_number(int number);
    void calculate_data_size_for_operand(std::shared_ptr<OperandBranch> branch);
    void calculate_operand_sizes_for_instruction(std::shared_ptr<InstructionBranch> instruction_branch);
    void add_must_fits_if_required(std::shared_ptr<InstructionBranch> ins_branch);
    
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
    void parse_global();
    void parse_data(DATA_BRANCH_TYPE data_branch_type = -1);

    inline bool is_next_valid_operand();
    inline bool is_next_segment();
    inline bool is_next_label();
    inline bool is_next_instruction();
    inline bool is_next_global();
    inline bool is_next_data();

    std::shared_ptr<Branch> root;

    std::shared_ptr<VirtualSegment> segment;
    std::vector<std::shared_ptr<VirtualSegment>> segments;

    std::shared_ptr<SegmentBranch> segment_branch;
    std::vector<std::shared_ptr<SegmentBranch>> segment_branches;

    Stream* sstream;
    std::shared_ptr<OperandBranch> left;
    std::shared_ptr<Branch> left_reg;
    std::shared_ptr<OperandBranch> right;
    std::shared_ptr<Branch> right_reg;
    
    std::shared_ptr<OffsetableBranch> last_offsetable_branch;

    INSTRUCTION_TYPE cur_ins_type;
    char mmm;
    char rrr;
    char oo;
    char op;
    int cur_offset;

    std::shared_ptr<OperandBranch> zero_operand_branch;


};

#endif /* ASSEMBLER8086_H */

