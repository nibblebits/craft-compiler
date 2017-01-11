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
 * File:   Assembler8086.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 16:44
 * 
 * Description: The assembly parser and assembler for the 8086 code generator
 * 
 * The assembler only supports one label per operand, this assembler is only intended for the code generator
 * and those using inline assembly. It is not designed for complicated assembling.
 * 
 * The assembler does not support segment registers as this code generator does not use segments.
 * Far jumps and access to memory in this type of way is also not yet supported, for example "jmp 0x00:0x00" is not yet supported,
 * however you can "jmp 0x00" just fine.
 * 
 * Please seek here: https://courses.engr.illinois.edu/ece390/resources/opcodes.html#Technical
 * for opcode information.
 */

#include "Assembler8086.h"
#include "common.h"
#include "def.h"
#include "LabelBranch.h"
#include "InstructionBranch.h"
#include "SegmentBranch.h"
#include "OperandBranch.h"
#include "EBranch.h"
#include "GlobalBranch.h"

/* The instruction map, maps the instruction enum to the correct opcodes. 
 * as some instructions share the same opcode */
unsigned char ins_map[] = {
    0x88, 0x89, 0xb1, 0xb8, 0xc6, 0xc7, 0xa2, 0xa3, 0xa0, 0xa1,
    0x8a, 0x8b, 0x88, 0x89, 0x00, 0x01, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x80, 0x81, 0x80, 0x81, 0x28, 0x29, 0x28, 0x29,
    0x2a, 0x2b, 0x2c, 0x2d, 0x80, 0x81, 0x80, 0x81, 0xf6, 0xf7,
    0xf6, 0xf7, 0xf6, 0xf7, 0xf6, 0xf7, 0xeb, 0xe9, 0xe8, 0x70,
    0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x50,
    0x58, 0xc3
};

// Full instruction size, related to opcode on the ins_map + what ever else is required for the instruction type
unsigned char ins_sizes[] = {
    2, 2, 2, 3, 3, 4, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 4, 3, 4, 2, 2, 2, 2,
    2, 2, 2, 3, 2, 3, 2, 3, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 3, 3, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
    1, 1
};


/* Holds the static "rrr" value for instructions only defining "oommm". 
 * if not applicable set to zero. */
unsigned char static_rrr[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 5, 5, 5, 5, 4, 4,
    4, 4, 6, 6, 6, 6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0
};

/* Describes information relating to an instruction 
 * Note: Only one of these may be chosen (HAS_RRR OR HAS_OOMMM OR HAS_OORRRMMM)
 * Also you may only use one of these (HAS_IMM_USE_LEFT OR HAS_IMM_USE_RIGHT)
 * ALSO NOTE: MEMORY IS GUIDED BY MODRM DO NOT USE (HAS_IMM_USE_LEFT OR HAS_IMM_USE_RIGHT) FOR MEMORY DISPLACEMENTS
 * WHILE AN OOMM IS PRESENT.*/

INSTRUCTION_INFO ins_info[] = {
    HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // mov reg8, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // mov reg16, reg16
    HAS_RRR | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // mov reg8, imm8
    USE_W | HAS_RRR | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // mov reg16, imm16
    HAS_OOMMM | HAS_IMM_USE_RIGHT, // mov mem, imm8
    USE_W | HAS_OOMMM | HAS_IMM_USE_RIGHT, // mov mem, imm16
    USE_W | HAS_IMM_USE_LEFT | HAS_REG_USE_RIGHT, // mov mem, al
    USE_W | HAS_IMM_USE_LEFT | HAS_REG_USE_RIGHT, // mov mem, ax
    USE_W | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // mov ax, mem
    USE_W | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // mov ax, mem
    HAS_OORRRMMM | HAS_REG_USE_LEFT, // mov reg8, mem
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT, // mov reg16, mem
    HAS_OORRRMMM | HAS_REG_USE_RIGHT, // mov mem, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_RIGHT, // mov mem, reg16
    HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // add reg8, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // add reg16, reg16
    HAS_OORRRMMM | HAS_REG_USE_RIGHT, // add mem, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_RIGHT, // add mem, reg16
    HAS_OORRRMMM | HAS_REG_USE_LEFT, // add reg8, mem
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT, // add reg16, mem
    HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // add al, imm8
    USE_W | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // add ax, imm16
    HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // add reg, imm8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // add reg, imm16
    HAS_OOMMM | HAS_IMM_USE_RIGHT, // add mem, imm8
    USE_W | HAS_OOMMM | HAS_IMM_USE_RIGHT, // add mem, imm16
    HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // sub reg8, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // sub reg16, reg16
    HAS_OORRRMMM | HAS_REG_USE_RIGHT, // sub mem, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_RIGHT, // sub mem, reg16
    HAS_OORRRMMM | HAS_REG_USE_LEFT, // sub reg8, mem
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT, // sub reg16, mem
    HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // sub al, imm8
    USE_W | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // sub ax, imm16
    HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // sub reg8, imm8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // sub reg16, imm16
    HAS_OOMMM | HAS_IMM_USE_RIGHT, // sub mem, imm8
    USE_W | HAS_OOMMM | HAS_IMM_USE_RIGHT, // sub mem, imm16
    HAS_OOMMM | HAS_REG_USE_LEFT, // mul reg8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT, // mul reg16
    HAS_OOMMM, // mul mem - byte specified in location is multiplied by AL
    USE_W | HAS_OOMMM, // mul mem - word specified in location is multiplied by AX
    HAS_OOMMM | HAS_REG_USE_LEFT, // div reg8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT, // div reg16
    HAS_OOMMM, // div mem - byte specified in location is divided by AL
    USE_W | HAS_OOMMM, // div mem - word specified in location is divided by AX
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE, // jmp short imm8
    USE_W | HAS_IMM_USE_LEFT | NEAR_POSSIBLE, // jmp near imm16
    USE_W | HAS_IMM_USE_LEFT | NEAR_POSSIBLE, // call near imm16
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // je short imm8
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // jne short imm8
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // jg short imm8
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // ja short imm8
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // jle short imm8
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // jbe short imm8
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // jl short imm8
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // jb short imm8
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // jge short imm8
    HAS_IMM_USE_LEFT | SHORT_POSSIBLE | USE_CONDITION_CODE, // jae short imm8
    USE_W | HAS_RRR | HAS_REG_USE_LEFT, // push reg16
    USE_W | HAS_RRR | HAS_REG_USE_LEFT, // pop reg16
    NO_PROPERTIES, // ret
};

struct ins_syntax_def ins_syntax[] = {
    "mov", MOV_REG_TO_REG_W0, REG8_REG8,
    "mov", MOV_REG_TO_REG_W1, REG16_REG16,
    "mov", MOV_IMM_TO_REG_W0, REG8_IMM8,
    "mov", MOV_IMM_TO_REG_W1, REG16_IMM16,
    "mov", MOV_IMM_TO_MEM_W0, MEM_IMM8,
    "mov", MOV_IMM_TO_MEM_W1, MEM_IMM16,
    "mov", MOV_ACC_TO_MEMOFFS_W0, MEM_AL,
    "mov", MOV_ACC_TO_MEMOFFS_W1, MEM_AX,
    "mov", MOV_REG_TO_MEM_W0, MEM_REG8,
    "mov", MOV_REG_TO_MEM_W1, MEM_REG16,
    "mov", MOV_MEM_TO_REG_W0, REG8_MEM,
    "mov", MOV_MEM_TO_REG_W1, REG16_MEM,
    "mov", MOV_REG_TO_MEM_W0, MEM_REG8,
    "mov", MOV_REG_TO_MEM_W1, MEM_REG16,
    "add", ADD_REG_WITH_REG_W0, REG8_REG8,
    "add", ADD_REG_WITH_REG_W1, REG16_REG16,
    "add", ADD_MEM_WITH_REG_W0, MEM_REG8,
    "add", ADD_MEM_WITH_REG_W1, MEM_REG16,
    "add", ADD_REG_WITH_MEM_W0, REG8_MEM,
    "add", ADD_REG_WITH_MEM_W1, REG16_MEM,
    "add", ADD_ACC_WITH_IMM_W0, AL_IMM8,
    "add", ADD_ACC_WITH_IMM_W1, AX_IMM16,
    "add", ADD_REG_WITH_IMM_W0, REG8_IMM8,
    "add", ADD_REG_WITH_IMM_W1, REG16_IMM16,
    "add", ADD_MEM_WITH_IMM_W0, MEM_IMM8,
    "add", ADD_MEM_WITH_IMM_W1, MEM_IMM16,
    "sub", SUB_REG_WITH_REG_W0, REG8_REG8,
    "sub", SUB_REG_WITH_REG_W1, REG16_REG16,
    "sub", SUB_MEM_WITH_REG_W0, MEM_REG8,
    "sub", SUB_MEM_WITH_REG_W1, MEM_REG16,
    "sub", SUB_REG_WITH_MEM_W0, REG8_MEM,
    "sub", SUB_REG_WITH_MEM_W1, REG16_MEM,
    "sub", SUB_ACC_WITH_IMM_W0, AL_IMM8,
    "sub", SUB_ACC_WITH_IMM_W1, AX_IMM16,
    "sub", SUB_REG_WITH_IMM_W0, REG8_IMM8,
    "sub", SUB_REG_WITH_IMM_W1, REG16_IMM16,
    "sub", SUB_MEM_WITH_IMM_W0, MEM_IMM8,
    "sub", SUB_MEM_WITH_IMM_W1, MEM_IMM16,
    "mul", MUL_WITH_REG_W0, REG8_ALONE,
    "mul", MUL_WITH_REG_W1, REG16_ALONE,
    "mul", MUL_WITH_MEM_W0, MEML8_ALONE,
    "mul", MUL_WITH_MEM_W1, MEML16_ALONE,
    "div", DIV_WITH_REG_W0, REG8_ALONE,
    "div", DIV_WITH_REG_W1, REG16_ALONE,
    "div", DIV_WITH_MEM_W0, MEML8_ALONE,
    "div", DIV_WITH_MEM_W1, MEML16_ALONE,
    "jmp", JMP_SHORT, IMM8_ALONE,
    "jmp", JMP_NEAR, IMM16_ALONE,
    "call", CALL_NEAR, IMM16_ALONE,
    "je", JE_SHORT, IMM8_ALONE,
    "jne", JNE_SHORT, IMM8_ALONE,
    "jg", JG_SHORT, IMM8_ALONE,
    "ja", JA_SHORT, IMM8_ALONE,
    "jle", JLE_SHORT, IMM8_ALONE,
    "jbe", JBE_SHORT, IMM8_ALONE,
    "jl", JL_SHORT, IMM8_ALONE,
    "jb", JB_SHORT, IMM8_ALONE,
    "jge", JL_SHORT, IMM8_ALONE,
    "jae", JB_SHORT, IMM8_ALONE,
    "push", PUSH_REG16, REG16_ALONE,
    "pop", POP_REG16, REG16_ALONE,
    "ret", RET, ALONE_ALONE
};

/* Certain instructions have condition codes that specify a particular event.
 * This array holds instruction names and the particular event associated with them */

struct condition_code_instruction cond_ins_code[] = {
    "je", EQUAL_ZERO,
    "jne", NOT_EQUAL_NOT_ZERO,
    "jg", GREATER_NOT_LESS_NOR_EQUAL,
    "ja", ABOVE_NOT_BELOW_NOR_EQUAL,
    "jle", LESS_OR_EQUAL_NOT_GREATER,
    "jbe", BELOW_OR_EQUAL_NOT_ABOVE,
    "jl", LESS_NOT_GREATER_NOR_EQUAL,
    "jb", CARRY_BELOW_NOT_ABOVE_NOR_EQUAL,
    "jge", GREATER_OR_EQUAL_NOT_LESS,
    "jae", NOT_CARRY_ABOVE_OR_EQUAL_NOT_BELOW
};

Assembler8086::Assembler8086(Compiler* compiler, std::shared_ptr<VirtualObjectFormat> object_format) : Assembler(compiler, object_format)
{
    Assembler::addKeyword("segment");
    Assembler::addKeyword("extern");
    Assembler::addKeyword("global");
    Assembler::addKeyword("byte");
    Assembler::addKeyword("word");

    Assembler::addRegister("ax");
    Assembler::addRegister("ah");
    Assembler::addRegister("al");
    Assembler::addRegister("cx");
    Assembler::addRegister("ch");
    Assembler::addRegister("cl");
    Assembler::addRegister("dx");
    Assembler::addRegister("dh");
    Assembler::addRegister("dl");
    Assembler::addRegister("bx");
    Assembler::addRegister("bh");
    Assembler::addRegister("bl");

    Assembler::addRegister("di");
    Assembler::addRegister("si");
    Assembler::addRegister("bp");
    Assembler::addRegister("sp");

    // Not all the instructions that are implemented, but enough for now
    Assembler::addInstruction("mov");
    Assembler::addInstruction("push");
    Assembler::addInstruction("pop");
    Assembler::addInstruction("add");
    Assembler::addInstruction("sub");
    Assembler::addInstruction("mul");
    Assembler::addInstruction("div");
    Assembler::addInstruction("xor");
    Assembler::addInstruction("and");
    Assembler::addInstruction("or");
    Assembler::addInstruction("int");
    Assembler::addInstruction("lea");
    Assembler::addInstruction("call");
    Assembler::addInstruction("jmp");
    Assembler::addInstruction("je");
    Assembler::addInstruction("jne");
    Assembler::addInstruction("jg");
    Assembler::addInstruction("ja");
    Assembler::addInstruction("jle");
    Assembler::addInstruction("jbe");
    Assembler::addInstruction("jl");
    Assembler::addInstruction("jb");
    Assembler::addInstruction("jge");
    Assembler::addInstruction("jae");
    Assembler::addInstruction("ret");
    Assembler::addInstruction("push");
    Assembler::addInstruction("pop");

    this->left = NULL;
    this->right = NULL;
    this->segment = NULL;
    this->sstream = NULL;
    this->cur_offset = 0;

    // Placeholder branch so programmer does not need to check if operand is NULL constantly.
    this->zero_operand_branch = std::shared_ptr<OperandBranch>(new OperandBranch(getCompiler()));

}

Assembler8086::~Assembler8086()
{
}

std::shared_ptr<Branch> Assembler8086::parse()
{
#ifdef DEBUG_MODE
    debug_output_tokens(Assembler::getTokens());
#endif

    root = std::shared_ptr<Branch>(new Branch("root", ""));
    while (hasTokens())
    {
        parse_part();
    }

    // Lets get all the branches
    while (hasBranches())
    {
        pop_front_branch();
        root->addChild(getPoppedBranch());
    }

#ifdef DEBUG_MODE
    debug_output_branch(root);
#endif

    return root;

}

void Assembler8086::exp_handler()
{
    shift();
}

void Assembler8086::left_exp_handler()
{
    exp_handler();
}

void Assembler8086::right_exp_handler()
{
    exp_handler();
}

std::shared_ptr<InstructionBranch> Assembler8086::new_ins_branch()
{
    return std::shared_ptr<InstructionBranch>(new InstructionBranch(getCompiler()));
}

void Assembler8086::parse_part()
{
    if (is_next_segment())
    {
        parse_segment();
    }
    else if (is_next_label())
    {
        parse_label();
    }
    else if (is_next_instruction())
    {
        parse_ins();
    }
    else if (is_next_global())
    {
        parse_global();
    }
    else
    {
        peek();
        throw AssemblerException("void Assembler8086::parse_part():  unexpected token \"" + getpeekTokenValue() + "\" this instruction or syntax may not be implemented.");
    }

}

std::shared_ptr<Branch> Assembler8086::get_identifier_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found)
{
    std::shared_ptr<Branch> result_branch = NULL;
    if (branch->getType() != "E")
    {
        if (branch->getType() == "identifier")
        {
            result_branch = branch;
        }
    }
    else
    {
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(branch);
        e_branch->iterate_expressions([&](std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) -> void
        {
            if (left_branch->getType() == "identifier")
            {
                result_branch = left_branch;
            }
            else if (right_branch->getType() == "identifier")
            {
                result_branch = right_branch;
            }
        });
    }

    if (remove_once_found)
    {
        if (result_branch != NULL)
        {
            result_branch->removeSelf();
        }
    }

    return result_branch;
}

std::shared_ptr<Branch> Assembler8086::get_register_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found)
{
    std::shared_ptr<Branch> result_branch = NULL;
    if (branch->getType() != "E")
    {
        if (branch->getType() == "register")
        {
            result_branch = branch;
        }
    }
    else
    {
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(branch);
        e_branch->iterate_expressions([&](std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) -> void
        {
            if (left_branch->getType() == "register")
            {
                result_branch = left_branch;
            }
            else if (right_branch->getType() == "register")
            {
                result_branch = right_branch;
            }
        });
    }

    if (remove_once_found)
    {
        if (result_branch != NULL)
        {
            result_branch->removeSelf();
        }
    }

    return result_branch;
}

std::shared_ptr<Branch> Assembler8086::get_number_branch_from_exp(std::shared_ptr<Branch> branch, bool remove_once_found)
{
    std::shared_ptr<Branch> result_branch = NULL;
    if (branch->getType() != "E")
    {
        if (branch->getType() == "number")
        {
            result_branch = branch;
        }
    }
    else
    {
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(branch);
        e_branch->iterate_expressions([&](std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) -> void
        {
            if (left_branch->getType() == "number")
            {
                result_branch = left_branch;
            }
            else if (right_branch->getType() == "number")
            {
                result_branch = right_branch;
            }
        });
    }

    if (remove_once_found)
    {
        if (result_branch != NULL)
        {
            result_branch->removeSelf();
        }
    }

    return result_branch;
}

void Assembler8086::handle_operand_exp(std::shared_ptr<OperandBranch> operand_branch)
{
    // Parse the expression
    parse_expression();
    pop_branch();
    // Now lets cut down the expression a bit and sum up the numbers
    std::shared_ptr<Branch> r_exp = sum_expression(getPoppedBranch());

    std::shared_ptr<Branch> number_branch = get_number_branch_from_exp(r_exp);
    std::shared_ptr<Branch> register_branch = get_register_branch_from_exp(r_exp);
    std::shared_ptr<Branch> identifier_branch = get_identifier_branch_from_exp(r_exp);

    // We clone because the framework does not allow the child to have two parents.
    if (number_branch != NULL)
        number_branch = number_branch->clone();
    if (register_branch != NULL)
        register_branch = register_branch->clone();
    if (identifier_branch != NULL)
        identifier_branch = identifier_branch->clone();

    operand_branch->setNumberBranch(number_branch);
    operand_branch->setRegisterBranch(register_branch);
    operand_branch->setIdentifierBranch(identifier_branch);
}

void Assembler8086::parse_operand(OPERAND_DATA_SIZE data_size)
{
    std::shared_ptr<OperandBranch> operand_branch = std::shared_ptr<OperandBranch>(new OperandBranch(getCompiler()));
    peek();
    if (is_peek_keyword("byte"))
    {
        if (data_size == OPERAND_DATA_SIZE_WORD)
        {
            throw AssemblerException("Error using byte with instruction while register indicates word");
        }
        // Ok we have byte access here
        shift_pop();
        operand_branch->setDataSize(OPERAND_DATA_SIZE_BYTE);
    }
    else if (is_peek_keyword("word"))
    {
        if (data_size == OPERAND_DATA_SIZE_BYTE)
        {
            throw AssemblerException("Error using word with instruction while register indicates byte");
        }
        // Ok a word access
        shift_pop();
        operand_branch->setDataSize(OPERAND_DATA_SIZE_WORD);
    }

    if (data_size != OPERAND_DATA_SIZE_UNKNOWN)
    {
        operand_branch->setDataSize(data_size);
    }

    peek();
    if (is_peek_symbol("["))
    {
        // Ok this will be memory access
        shift_pop();
        operand_branch->setMemoryAccess(true);
        handle_operand_exp(operand_branch);
        // Ok ending memory access shift off the "]" token.
        shift_pop();
    }
    else
    {
        handle_operand_exp(operand_branch);
    }



    push_branch(operand_branch);
}

void Assembler8086::parse_segment()
{
    // Shift and pop the segment name we don't need it anymore
    shift_pop();

    // Next up is the segment name, we need this
    shift_pop();
    std::shared_ptr<Branch> segment_name_branch = Assembler::getPoppedBranch();

    std::shared_ptr<SegmentBranch> segment_root = std::shared_ptr<SegmentBranch>(new SegmentBranch(getCompiler()));
    segment_root->setSegmentNameBranch(segment_name_branch);

    // Create the contents branch and add it to the segment branch
    std::shared_ptr<Branch> contents_branch = std::shared_ptr<Branch>(new Branch("CONTENTS", ""));
    segment_root->setContentsBranch(contents_branch);

    while (hasTokens())
    {
        if (is_next_segment())
        {
            /* We should stop now as we are at another segment, segments are independent from other segments*/
            break;
        }

        parse_part();
        pop_branch();
        // Add the branch to the segment contents branch
        contents_branch->addChild(getPoppedBranch());
    }

    push_branch(segment_root);

}

void Assembler8086::parse_label()
{
    // Shift and pop the label name
    shift_pop();
    std::shared_ptr<Branch> label_name_branch = Assembler::getPoppedBranch();
    // Shift and pop the symbol ":" we do not need it anymore
    shift_pop();

    // Create the label branch
    std::shared_ptr<LabelBranch> label_branch = std::shared_ptr<LabelBranch>(new LabelBranch(getCompiler()));
    label_branch->setLabelNameBranch(label_name_branch);

    // Create the label contents branch and add it to the label branch
    std::shared_ptr<Branch> label_contents_branch = std::shared_ptr<Branch>(new Branch("CONTENTS", ""));
    label_branch->setContentsBranch(label_contents_branch);

    while (hasTokens())
    {
        if (is_next_label() ||
                is_next_segment())
        {
            /* We should stop now as we are at another label or segment
             all labels and segments should be independent from other labels and segments.*/
            break;
        }

        parse_part();
        pop_branch();
        // Add the branch to the label contents branch
        label_contents_branch->addChild(getPoppedBranch());
    }

    push_branch(label_branch);
}

void Assembler8086::parse_ins()
{
    std::shared_ptr<Branch> name_branch = NULL;
    std::shared_ptr<OperandBranch> dest_op = NULL;
    std::shared_ptr<OperandBranch> source_op = NULL;

    // Shift and pop the instruction
    shift_pop();
    name_branch = getPoppedBranch();

    // Do we have an expression
    peek();
    if (is_next_valid_operand())
    {
        // Next will be the left operand
        parse_operand();
        // Pop it off
        pop_branch();
        dest_op = std::dynamic_pointer_cast<OperandBranch>(getPoppedBranch());

        // Do we have a second operand?
        peek();
        if (is_peek_symbol(","))
        {
            // Now we need to shift and pop off the comma ","
            shift_pop();

            OPERAND_DATA_SIZE def_data_size = OPERAND_DATA_SIZE_UNKNOWN;
            if (dest_op->hasRegisterBranch())
            {
                if (is_reg_16_bit(dest_op->getRegisterBranch()->getValue()))
                {
                    def_data_size = OPERAND_DATA_SIZE_WORD;
                }
                else
                {
                    def_data_size = OPERAND_DATA_SIZE_BYTE;
                }
            }

            // Finally a final expression which will be the second operand
            parse_operand(def_data_size);

            // Pop it off
            pop_branch();
            source_op = std::dynamic_pointer_cast<OperandBranch>(getPoppedBranch());
        }
    }

    // Put it all together
    std::shared_ptr<InstructionBranch> ins_branch = std::shared_ptr<InstructionBranch>(new InstructionBranch(getCompiler()));
    ins_branch->setInstructionNameBranch(name_branch);
    ins_branch->setLeftBranch(dest_op);
    ins_branch->setRightBranch(source_op);

    // Push the finished branch to the stack
    push_branch(ins_branch);
}

void Assembler8086::parse_global()
{
    // Shift and pop the global keyword
    shift_pop();
    std::shared_ptr<GlobalBranch> global_branch = std::shared_ptr<GlobalBranch>(new GlobalBranch(getCompiler()));

    // Shift and pop the label name branch
    peek();
    if (!is_peek_type("identifier"))
    {
        throw Exception("void Assembler8086::parse_global(): expecting an identifier but none was provided.");
    }

    // Shift and pop off the identifier
    shift_pop();
    global_branch->setLabelNameBranch(getPoppedBranch());

    // Push the global branch to the stack
    push_branch(global_branch);
}

bool Assembler8086::is_next_valid_operand()
{
    return (is_peek_type("identifier")
            || is_peek_type("number")
            || is_peek_type("register")
            || is_peek_symbol("[")
            || is_peek_type("keyword"));
}

bool Assembler8086::is_next_segment()
{
    peek();
    return (is_peek_type("keyword")
            && is_peek_value("segment"));
}

bool Assembler8086::is_next_label()
{
    peek();
    if (is_peek_type("identifier"))
    {
        peek(1);
        if (is_peek_symbol(":"))
        {
            // This is a label
            return true;
        }
    }

    return false;
}

bool Assembler8086::is_next_instruction()
{
    peek();
    return is_peek_type("instruction");
}

bool Assembler8086::is_next_global()
{
    peek();
    return is_peek_keyword("global");
}

void Assembler8086::generate()
{
    assembler_pass_1();
    assembler_pass_2();

    std::cout << this->cur_offset << std::endl;
}

void Assembler8086::assembler_pass_1()
{
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        if (branch->getType() == "SEGMENT")
        {
            // Reset the current offset ready for the next segment
            this->cur_offset = 0;
            pass_1_segment(std::dynamic_pointer_cast<SegmentBranch>(branch));
        }
        else
        {
            throw new AssemblerException("void Assembler8086::generate(): branch requires a segment.");
        }
    }
}

void Assembler8086::pass_1_segment(std::shared_ptr<SegmentBranch> segment_branch)
{
    // Register the segment
    register_segment(segment_branch);

    // Now we need to pass through the children
    for (std::shared_ptr<Branch> child : segment_branch->getContentsBranch()->getChildren())
    {
        pass_1_part(child);
    }
}

void Assembler8086::pass_1_part(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "LABEL")
    {
        // This is a label, therefore we need to give it, its position.
        std::shared_ptr<LabelBranch> label_branch = std::dynamic_pointer_cast<LabelBranch>(branch);
        label_branch->setOffset(this->cur_offset);
        // Now we need to pass through the children
        for (std::shared_ptr<Branch> child : label_branch->getContentsBranch()->getChildren())
        {
            pass_1_part(child);
        }
    }
    else if (branch->getType() == "INSTRUCTION")
    {
        std::shared_ptr<InstructionBranch> ins_branch = std::dynamic_pointer_cast<InstructionBranch>(branch);
        ins_branch->setOffset(this->cur_offset);
        int size = get_instruction_size(std::dynamic_pointer_cast<InstructionBranch>(branch));
        ins_branch->setSize(size);
        this->cur_offset += size;
    }
    else if (branch->getType() == "GLOBAL")
    {

    }
    else
    {
        throw AssemblerException("void Assembler8086::pass_1_part(std::shared_ptr<Branch> branch): "
                                 "expecting label, instruction or keyword but a \"" + branch->getType() + "\" was provided");
    }
}

void Assembler8086::get_modrm_from_instruction(std::shared_ptr<InstructionBranch> ins_branch, char* oo, char* rrr, char* mmm)
{
    INSTRUCTION_TYPE ins_type = get_instruction_type(ins_branch);
    if (ins_type == -1)
    {
        throw AssemblerException("void Assembler8086::get_modrm_from_instruction(std::shared_ptr<InstructionBranch> ins_branch, char* oo, char* rrr, char* mmm): Problem invalid instruction");
    }
    INSTRUCTION_INFO info = ins_info[ins_type];
    unsigned int number;
    left = zero_operand_branch;
    right = zero_operand_branch;

    if (ins_branch->hasLeftBranch())
    {
        left = ins_branch->getLeftBranch();
        if (left->hasRegisterBranch())
        {
            left_reg = left->getRegisterBranch();
        }
    }

    if (ins_branch->hasRightBranch())
    {
        right = ins_branch->getRightBranch();
        if (right->hasRegisterBranch())
        {
            right_reg = right->getRegisterBranch();
        }
    }

    *rrr = -1;
    *mmm = -1;
    if (info & HAS_OOMMM)
    {
        if ((!right->isAccessingMemory() && left->isOnlyRegister()) ||
                (!left->isAccessingMemory() && right->isOnlyRegister()))
        {
            *oo = USE_REG_NO_ADDRESSING_MODE;
        }
        else
        {
            *oo = DISPLACEMENT_IF_MMM_110;
            *mmm = 0b110;
        }
        // Only OOMMM available, must be using static RRR
        *rrr = static_rrr[ins_type];
    }
    else if (left->isOnlyRegister()
            && right->isOnlyRegister())
    {
        *oo = USE_REG_NO_ADDRESSING_MODE;
    }
    else
    {
        *oo = -1;
    }

    if (*oo == -1)
    {
        if (left->isAccessingMemory()
                && left->hasImmediate())
        {
            if (!left->hasRegisterBranch() &&
                    right->isOnlyRegister())
            {
                *oo = DISPLACEMENT_IF_MMM_110;
                *mmm = 0b110;
            }
            else
            {
                if (left->hasIdentifierBranch())
                {
                    *oo = DISPLACEMENT_16BIT_FOLLOW;
                }
                else if (left->hasNumberBranch())
                {
                    number = std::stoi(left->getNumberBranch()->getValue());
                    if (number < 256)
                    {
                        *oo = DISPLACEMENT_8BIT_FOLLOW;
                    }
                    else
                    {
                        *oo = DISPLACEMENT_16BIT_FOLLOW;
                    }
                }
            }
        }
        else if (right->isAccessingMemory()
                && right->hasImmediate())
        {
            if (!right->hasRegisterBranch()
                    && left->isOnlyRegister())
            {
                *oo = DISPLACEMENT_IF_MMM_110;
                *mmm = 0b110;
            }
            else
            {
                if (right->hasIdentifierBranch())
                {
                    *oo = DISPLACEMENT_16BIT_FOLLOW;
                }
                else if (right->hasNumberBranch())
                {
                    number = std::stoi(right->getNumberBranch()->getValue());
                    if (number < 256)
                    {
                        *oo = DISPLACEMENT_8BIT_FOLLOW;
                    }
                    else
                    {
                        *oo = DISPLACEMENT_16BIT_FOLLOW;
                    }
                }
            }
        }
        else
        {
            *oo = DISPLACEMENT_IF_MMM_110;
            *mmm = 0b110;
        }
    }

    if (left->hasRegisterBranch())
    {
        if (left->isAccessingMemory())
        {
            *mmm = get_mmm(left_reg->getValue());
        }
        else
        {
            if (*oo == DISPLACEMENT_IF_MMM_110 ||
                    *oo == DISPLACEMENT_8BIT_FOLLOW ||
                    *oo == DISPLACEMENT_16BIT_FOLLOW)
            {
                *rrr = get_reg(left_reg->getValue());
            }
            else
            {
                *mmm = get_reg(left_reg->getValue());
            }

        }
    }

    if (right->hasRegisterBranch())
    {
        if (right->isAccessingMemory())
        {
            if (*oo == DISPLACEMENT_IF_MMM_110 ||
                    *oo == DISPLACEMENT_8BIT_FOLLOW ||
                    *oo == DISPLACEMENT_16BIT_FOLLOW)
            {
                *mmm = get_mmm(right_reg->getValue());
            }
            else
            {
                *rrr = get_mmm(right_reg->getValue());
            }
        }
        else
        {
            *rrr = get_reg(right_reg->getValue());
        }
    }

}

int Assembler8086::get_offset_from_oomod(char oo, char mmm)
{
    int offset = 0;
    if (oo == DISPLACEMENT_IF_MMM_110)
    {
        if (mmm == 0b110)
        {
            offset += 2;
        }
    }
    else if (oo == DISPLACEMENT_8BIT_FOLLOW)
    {
        offset += 1;
    }
    else if (oo == DISPLACEMENT_16BIT_FOLLOW)
    {
        offset += 2;
    }

    return offset;
}

int Assembler8086::get_instruction_size(std::shared_ptr<InstructionBranch> ins_branch)
{
    int ins_type = get_instruction_type(ins_branch);
    int size = ins_sizes[ins_type];

    if (has_oommm(ins_type))
    {
        get_modrm_from_instruction(ins_branch, &oo, &rrr, &mmm);
        size += get_offset_from_oomod(oo, mmm);
    }
    return size;
}

void Assembler8086::register_segment(std::shared_ptr<SegmentBranch> segment_branch)
{
    std::shared_ptr<VirtualObjectFormat> obj_format = Assembler::getObjectFormat();
    std::shared_ptr<VirtualSegment> segment = obj_format->createSegment(segment_branch->getSegmentNameBranch()->getValue());
    this->segments.push_back(segment);
}

void Assembler8086::switch_to_segment(std::string segment_name)
{
    for (std::shared_ptr<VirtualSegment> segment : this->segments)
    {
        if (segment->getName() == segment_name)
        {
            this->segment = segment;
            this->sstream = this->segment->getStream();
            return;
        }
    }

    throw AssemblerException("void Assembler8086::switch_to_segment(std::string segment_name): \"" + segment_name + "\" does not exist");
}

void Assembler8086::assembler_pass_2()
{
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        if (branch->getType() == "SEGMENT")
        {
            generate_segment(std::dynamic_pointer_cast<SegmentBranch>(branch));
        }
        else
        {
            throw new AssemblerException("void Assembler8086::generate(): branch requires a segment.");
        }
    }
}

void Assembler8086::handle_rrr(int* opcode, INSTRUCTION_INFO info, std::shared_ptr<InstructionBranch> ins_branch)
{
    std::shared_ptr<Branch> selected_reg = NULL;
    if (info & HAS_REG_USE_LEFT)
    {
        selected_reg = ins_branch->getLeftBranch()->getRegisterBranch();
    }
    else if (info & HAS_REG_USE_RIGHT)
    {
        selected_reg = ins_branch->getRightBranch()->getRegisterBranch();
    }
    else
    {
        ins_info_except();
    }

    *opcode |= get_reg(selected_reg->getValue());
}

void Assembler8086::handle_condition_code(int* opcode, std::shared_ptr<InstructionBranch> ins_branch)
{
    CONDITION_CODE cond_code = get_condition_code_for_instruction(ins_branch->getInstructionNameBranch()->getValue());
    // We must now apply it to the opcode
    *opcode |= cond_code;
}

void Assembler8086::gen_oommm(INSTRUCTION_TYPE ins_type, std::shared_ptr<InstructionBranch> ins_branch)
{
    // What is the static RRR here?
    rrr = static_rrr[ins_type];
    // Generate the complete oorrrmmm
    gen_oorrrmmm(ins_branch, rrr);
}

void Assembler8086::gen_oorrrmmm(std::shared_ptr<InstructionBranch> ins_branch, unsigned char def_rrr)
{
    get_modrm_from_instruction(ins_branch, &oo, &rrr, &mmm);

    // Do we have a default rrr?
    if (def_rrr == -1)
    {
        rrr = def_rrr;
    }

    // Write the MODRM
    sstream->write8(bind_modrm(oo, rrr, mmm));

    // Find out the branch which an modrm offset is required for (if any)
    if (oo != USE_REG_NO_ADDRESSING_MODE)
    {
        std::shared_ptr<OperandBranch> selected_branch = NULL;
        if (ins_branch->hasLeftBranch()
                && ins_branch->getLeftBranch()->isAccessingMemory())
        {
            selected_branch = ins_branch->getLeftBranch();
        }
        else
        {
            selected_branch = ins_branch->getRightBranch();
        }

        // Write the offset 
        write_modrm_offset(oo, mmm, selected_branch);
    }
}

void Assembler8086::gen_imm(INSTRUCTION_INFO info, std::shared_ptr<InstructionBranch> ins_branch)
{
    std::shared_ptr<OperandBranch> selected_operand = NULL;
    if (info & HAS_IMM_USE_LEFT)
    {
        selected_operand = ins_branch->getLeftBranch();
    }
    else
    {
        selected_operand = ins_branch->getRightBranch();
    }

    if (info & USE_W)
    {
        if (info & NEAR_POSSIBLE)
        {
            write_abs_static16(selected_operand, true, ins_branch);
        }
        else
        {
            write_abs_static16(selected_operand);
        }
    }
    else
    {
        if (info & SHORT_POSSIBLE)
        {
            write_abs_static8(selected_operand, true, ins_branch);
        }
        else
        {
            write_abs_static8(selected_operand);
        }
    }
}

void Assembler8086::generate_part(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "LABEL")
    {
        std::shared_ptr<LabelBranch> label_branch = std::dynamic_pointer_cast<LabelBranch>(branch);
        for (std::shared_ptr<Branch> child : label_branch->getContentsBranch()->getChildren())
        {
            generate_part(child);
        }
    }
    else if (branch->getType() == "INSTRUCTION")
    {
        generate_instruction(std::dynamic_pointer_cast<InstructionBranch>(branch));
    }


}

void Assembler8086::generate_instruction(std::shared_ptr<InstructionBranch> instruction_branch)
{
    std::string ins_name = instruction_branch->getInstructionNameBranch()->getValue();
    INSTRUCTION_TYPE ins_type = get_instruction_type(instruction_branch);
    cur_ins_type = ins_type;

    int opcode = ins_map[ins_type];
    INSTRUCTION_INFO info = ins_info[ins_type];

    if (info & HAS_RRR)
    {
        handle_rrr(&opcode, info, instruction_branch);
    }
    else if (info & USE_CONDITION_CODE)
    {
        handle_condition_code(&opcode, instruction_branch);
    }

    // Write the opcode
    sstream->write8(opcode);
    if (info & HAS_OOMMM)
    {
        gen_oommm(ins_type, instruction_branch);
    }
    else if (info & HAS_OORRRMMM)
    {
        gen_oorrrmmm(instruction_branch);
    }

    if (info & HAS_IMM_USE_LEFT ||
            info & HAS_IMM_USE_RIGHT)
    {
        gen_imm(info, instruction_branch);
    }
}

char Assembler8086::bind_modrm(char oo, char rrr, char mmm)
{
    return (oo << 6 | rrr << 3 | mmm);
}

bool Assembler8086::has_oommm(INSTRUCTION_TYPE ins_type)
{
    unsigned char info = ins_info[ins_type];
    if (info & HAS_OOMMM ||
            info & HAS_OORRRMMM)
    {
        return true;
    }

    return false;
}

CONDITION_CODE Assembler8086::get_condition_code_for_instruction(std::string instruction_name)
{
    int size = sizeof (cond_ins_code) / sizeof (struct condition_code_instruction);
    for (int i = 0; i < size; i++)
    {
        if (cond_ins_code[i].ins_name == instruction_name)
        {
            return cond_ins_code[i].code;
        }
    }

    return -1;
}

int Assembler8086::get_static_from_branch(std::shared_ptr<OperandBranch> branch, bool short_or_near_possible, std::shared_ptr<InstructionBranch> ins_branch)
{
    int value = 0;
    if (branch->hasNumberBranch())
    {
        value += std::stoi(branch->getNumberBranch()->getValue());
    }

    if (branch->hasIdentifierBranch())
    {
        int lbl_offset = get_label_offset(branch->getIdentifierBranch()->getValue());
        if (short_or_near_possible)
        {
            // Ok its possible to deal with this as a short so lets do that, this is valid with things such as short jumps.  
            int ins_offset = ins_branch->getOffset();
            int ins_size = ins_branch->getSize();
            int abs = lbl_offset - ins_offset - ins_size;
            value += abs;
        }
        else
        {
            value += lbl_offset;
        }
    }

    return value;
}

void Assembler8086::write_modrm_offset(unsigned char oo, unsigned char mmm, std::shared_ptr<OperandBranch> branch)
{
    switch (oo)
    {
    case DISPLACEMENT_IF_MMM_110:
        if (mmm == 0b110)
        {
            write_abs_static16(branch);
        }
        break;
    case DISPLACEMENT_8BIT_FOLLOW:
        write_abs_static8(branch);
        break;
    case DISPLACEMENT_16BIT_FOLLOW:
        write_abs_static16(branch);
        break;
    }
}

void Assembler8086::write_abs_static8(std::shared_ptr<OperandBranch> branch, bool short_possible, std::shared_ptr<InstructionBranch> ins_branch)
{
    sstream->write8(get_static_from_branch(branch, short_possible, ins_branch));
}

void Assembler8086::write_abs_static16(std::shared_ptr<OperandBranch> branch, bool near_possible, std::shared_ptr<InstructionBranch> ins_branch)
{
    sstream->write16(get_static_from_branch(branch, near_possible, ins_branch));
}

std::shared_ptr<LabelBranch> Assembler8086::get_label_branch(std::string label_name)
{
    for (std::shared_ptr<Branch> child : segment_branch->getContentsBranch()->getChildren())
    {
        if (child->getType() == "LABEL")
        {
            std::shared_ptr<LabelBranch> lbl_branch = std::dynamic_pointer_cast<LabelBranch>(child);
            if (lbl_branch->getLabelNameBranch()->getValue() == label_name)
                return lbl_branch;
        }
    }

    return NULL;
}

int Assembler8086::get_label_offset(std::string label_name)
{
    return get_label_branch(label_name)->getOffset();
}

void Assembler8086::generate_segment(std::shared_ptr<SegmentBranch> branch)
{
    this->segment_branch = branch;
    switch_to_segment(branch->getSegmentNameBranch()->getValue());
    for (std::shared_ptr<Branch> child : branch->getContentsBranch()->getChildren())
    {
        generate_part(child);
    }
}

OPERAND_INFO Assembler8086::get_operand_info(std::shared_ptr<OperandBranch> op_branch)
{
    OPERAND_INFO info = -1;
    std::shared_ptr<Branch> reg_branch = NULL;
    if (op_branch->isOnlyRegister())
    {
        reg_branch = op_branch->getRegisterBranch();
        if (is_accumulator_and_not_ah(reg_branch->getValue()))
        {
            if (is_reg_16_bit(reg_branch->getValue()))
            {
                info = AX;
            }
            else
            {
                info = AL;
            }
        }
        else
        {
            if (is_reg_16_bit(reg_branch->getValue()))
            {
                info = REG16;
            }
            else
            {
                info = REG8;
            }
        }
    }
    else if (op_branch->isOnlyImmediate())
    {
        if (op_branch->getDataSize() == OPERAND_DATA_SIZE_UNKNOWN &&
                op_branch->hasIdentifierBranch())
        {
            /* This operand has a label we should set it to a near jump for now
             * Note this will cause problems if label is out of range for the near jump.
             * In the future this must be changed to pick the appropriate jump type seek here: http://stackoverflow.com/questions/41418521/assembler-passes-issue*/
            info = IMM8;
        }
        else
        {
            if (op_branch->getDataSize() == OPERAND_DATA_SIZE_WORD)
            {
                info = IMM16;
            }
            else
            {
                info = IMM8;
            }
        }
    }
    else if (op_branch->isAccessingMemory())
    {
        // If a data size is provided then we must be loading bits of 8 or 16
        if (op_branch->getDataSize() == OPERAND_DATA_SIZE_BYTE)
        {
            info = MEML8;
        }
        else if (op_branch->getDataSize() == OPERAND_DATA_SIZE_WORD)
        {
            info = MEML16;
        }
        else
        {
            info = MEM;
        }
    }

    return info;
}

SYNTAX_INFO Assembler8086::get_syntax_info(std::shared_ptr<InstructionBranch> instruction_branch, OPERAND_INFO* left_op, OPERAND_INFO* right_op)
{
    *left_op = ALONE;
    *right_op = ALONE;
    if (instruction_branch->hasLeftBranch())
    {
        *left_op = get_operand_info(instruction_branch->getLeftBranch());
    }
    if (instruction_branch->hasRightBranch())
    {
        *right_op = get_operand_info(instruction_branch->getRightBranch());
    }

    return (*left_op << OPERAND_BIT_SIZE | *right_op);
}

INSTRUCTION_TYPE Assembler8086::get_instruction_type_by_name_and_syntax(std::string instruction_name, SYNTAX_INFO syntax_info)
{
    int total = sizeof (ins_syntax) / sizeof (struct ins_syntax_def);
    for (int i = 0; i < total; i++)
    {
        struct ins_syntax_def* i_syn = &ins_syntax[i];
        if (i_syn->ins_name == instruction_name)
        {
            if (i_syn->syntax_info == syntax_info)
            {
                return i_syn->ins_type;
            }
        }
    }

    return -1;
}

INSTRUCTION_TYPE Assembler8086::get_instruction_type(std::shared_ptr<InstructionBranch> instruction_branch)
{
    INSTRUCTION_TYPE ins_type;
    OPERAND_INFO left_op;
    OPERAND_INFO right_op;

    std::string instruction_name = instruction_branch->getInstructionNameBranch()->getValue();
    // We need to build the syntax and then try and pull out the correct result from an array
    SYNTAX_INFO syntax_info = get_syntax_info(instruction_branch, &left_op, &right_op);

    // Ok lets get the instruction type now that we have the syntax info
    ins_type = get_instruction_type_by_name_and_syntax(instruction_name, syntax_info);
    if (ins_type == -1)
    {
        /* We couldn't find an instruction, perhaps the syntax is using AL or AX rather than REG8 and REG16 
         * and no appropriate instruction exists for it, lets check and if so change to REG8 or REG16 and try again*/
        if (left_op == AL)
        {
            left_op = REG8;
        }
        else if (left_op == AX)
        {
            left_op = REG16;
        }

        if (right_op == AL)
        {
            right_op = REG8;
        }
        else if (right_op == AX)
        {
            right_op = REG16;
        }

        // Rebuild the syntax info
        syntax_info = (left_op << OPERAND_BIT_SIZE | right_op);

        // Now try again
        ins_type = get_instruction_type_by_name_and_syntax(instruction_name, syntax_info);
        if (ins_type == -1)
        {
            // There truly is a problem
            throw AssemblerException("The instruction: " + instruction_name + " does not exist or is not implemented or you are using an illegal syntax");
        }
    }

    return ins_type;

}

INSTRUCTION_TYPE Assembler8086::get_mov_ins_type(std::shared_ptr<InstructionBranch> instruction_branch)
{
    left = instruction_branch->getLeftBranch();
    right = instruction_branch->getRightBranch();

    if (left->isOnlyRegister())
    {
        left_reg = left->getRegisterBranch();
        if (right->isOnlyRegister())
        {
            // Register to register assignment "mov reg, reg", 8 bit or 16 bit assignment?
            if (is_reg_16_bit(left_reg->getValue()))
            {
                // 16 bit assignment here
                return MOV_REG_TO_REG_W1;
            }
            else
            {
                // Must be an 8 bit assignment.
                return MOV_REG_TO_REG_W0;
            }
        }
        else if (right->isOnlyImmediate())
        {
            // Register to register assignment, 8 bit or 16 bit assignment?
            if (is_reg_16_bit(left_reg->getValue()))
            {
                // 16 bit assignment here
                return MOV_IMM_TO_REG_W1;
            }
            else
            {
                // Must be an 8 bit assignment.
                return MOV_IMM_TO_REG_W0;
            }
        }
        else if (right->isAccessingMemory())
        {
            // Is this the accumulator we are going to store this into?
            if (is_accumulator_and_not_ah(left_reg->getValue()))
            {
                if (is_reg_16_bit(left_reg->getValue()))
                {
                    return MOV_MEMOFFS_TO_ACC_W1;
                }
                else
                {
                    return MOV_MEMOFFS_TO_ACC_W0;
                }
            }
            else
            {
                if (is_reg_16_bit(left_reg->getValue()))
                {
                    return MOV_MEM_TO_REG_W1;
                }
                else
                {
                    return MOV_MEM_TO_REG_W0;
                }
            }
        }
    }
    else if (left->isAccessingMemory())
    {
        // Memory assignment.
        if (right->isOnlyRegister())
        {
            right_reg = right->getRegisterBranch();
            // mov mem, reg
            // Is this the accumulator we are referring to?
            if (is_accumulator_and_not_ah(right_reg->getValue()))
            {
                if (is_reg_16_bit(right_reg->getValue()))
                {
                    return MOV_ACC_TO_MEMOFFS_W1;
                }
                else
                {
                    return MOV_ACC_TO_MEMOFFS_W0;
                }
            }
            else
            {
                if (is_reg_16_bit(right_reg->getValue()))
                {
                    return MOV_REG_TO_MEM_W1;
                }
                else
                {
                    return MOV_REG_TO_MEM_W0;
                }
            }
        }
        else if (right->isOnlyImmediate())
        {
            // mov mem, imm
            if (left->getDataSize() == OPERAND_DATA_SIZE_WORD)
            {
                return MOV_IMM_TO_MEM_W1;
            }
            else
            {
                return MOV_IMM_TO_MEM_W0;
            }
        }
    }
}

INSTRUCTION_TYPE Assembler8086::get_add_ins_type(std::shared_ptr<InstructionBranch> instruction_branch)
{
    left = instruction_branch->getLeftBranch();
    right = instruction_branch->getRightBranch();

    if (left->isOnlyRegister())
    {
        left_reg = left->getRegisterBranch();
        if (right->isOnlyRegister())
        {
            if (is_reg_16_bit(left_reg->getValue()))
            {
                return ADD_REG_WITH_REG_W1;
            }
            else
            {
                return ADD_REG_WITH_REG_W0;
            }
        }
        else if (right->isAccessingMemory())
        {
            if (is_reg_16_bit(left_reg->getValue()))
            {
                return ADD_REG_WITH_MEM_W1;
            }
            else
            {
                return ADD_REG_WITH_MEM_W0;
            }
        }
    }
    else if (right->isOnlyRegister())
    {
        right_reg = right->getRegisterBranch();
        if (left->isAccessingMemory())
        {
            if (right->isOnlyRegister())
            {
                if (is_reg_16_bit(right_reg->getValue()))
                {
                    return ADD_MEM_WITH_REG_W1;
                }
                else
                {
                    return ADD_MEM_WITH_REG_W0;
                }
            }
        }
    }

    return -1;
}

bool Assembler8086::is_accumulator_and_not_ah(std::string _register)
{
    return _register == "al"
            || _register == "ax";
}

bool Assembler8086::is_reg(std::string _register)
{
    return (_register == "al"
            || _register == "ah"
            || _register == "ax"
            || _register == "bh"
            || _register == "bl"
            || _register == "bx"
            || _register == "ch"
            || _register == "cl"
            || _register == "cx"
            || _register == "dh"
            || _register == "dl"
            || _register == "dx");
}

char Assembler8086::get_reg(std::string _register)
{
    if (_register == "al" || _register == "ax")
    {
        return 0;
    }
    else if (_register == "cl" || _register == "cx")
    {
        return 1;
    }
    else if (_register == "dl" || _register == "dx")
    {
        return 2;
    }
    else if (_register == "bl" || _register == "bx")
    {
        return 3;
    }
    else if (_register == "ah" || _register == "sp")
    {
        return 4;
    }
    else if (_register == "ch" || _register == "bp")
    {
        return 5;
    }
    else if (_register == "dh" || _register == "si")
    {
        return 6;
    }
    else if (_register == "bh" || _register == "di")
    {
        return 7;
    }
}

bool Assembler8086::is_mmm(std::string _register, std::string second_reg)
{
    return get_mmm(_register, second_reg) != -1;
}

char Assembler8086::get_mmm(std::string _register, std::string second_reg)
{
    if (_register == "bx" && second_reg == "si")
    {
        return 0;
    }
    else if (_register == "bx" && second_reg == "di")
    {
        return 1;
    }
    else if (_register == "bp" && second_reg == "si")
    {
        return 2;
    }
    else if (_register == "bp" && second_reg == "di")
    {
        return 3;
    }
    else if (_register == "si")
    {
        return 4;
    }
    else if (_register == "di")
    {
        return 5;
    }
    else if (_register == "bp")
    {
        return 6;
    }
    else if (_register == "bx")
    {
        return 7;
    }
    else
    {
        return -1;
    }
}

bool Assembler8086::is_reg_16_bit(std::string _register)
{
    return (_register == "ax" ||
            _register == "bx" ||
            _register == "cx" ||
            _register == "dx" ||
            _register == "sp" ||
            _register == "bp" ||
            _register == "ss" ||
            _register == "ds" ||
            _register == "es" ||
            _register == "es");
}

void Assembler8086::ins_info_except()
{
    throw AssemblerException("Improperly formatted ins_info array for instruction type: " + std::to_string(cur_ins_type));
}