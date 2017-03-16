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
#include "ExternBranch.h"
#include "DataBranch.h"
#include "OffsetableBranch.h"
#include "MustFitTable.h"


#ifdef DEBUG_MODE

const char* operand_info_str[] = {
    "REG8",
    "REG16",
    "AL",
    "AX",
    "MEM8",
    "MEM16",
    "IMM8",
    "IMM16",
    "ALONE"
};

#endif
/* The instruction map, maps the instruction enum to the correct opcodes. 
 * as some instructions share the same opcode */
unsigned char ins_map[] = {
    0x88, 0x89, 0xb0, 0xb8, 0xc6, 0xc7, 0x8a, 0x8b, 0x88, 0x89,
    0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x80, 0x81,
    0x80, 0x81, 0x28, 0x29, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
    0x80, 0x81, 0x80, 0x81, 0xf6, 0xf7, 0xf6, 0xf7, 0xf6, 0xf7,
    0xf6, 0xf7, 0xe9, 0xe8, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
    0x70, 0x70, 0x70, 0x70, 0x50, 0x58, 0xc3, 0x30, 0x31, 0x30,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x80, 0x81, 0x80, 0x81, 0x08,
    0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x80, 0x81, 0x80,
    0x81, 0x20, 0x21, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x80,
    0x81, 0x80, 0x81, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0,
    0xc1, 0xcd, 0x38, 0x39, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,
    0x80, 0x81, 0x80, 0x81, 0x8d
};

// Full instruction size, related to opcode on the ins_map + what ever else is required for the instruction type
unsigned char ins_sizes[] = {
    2, 2, 2, 3, 5, 6, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 3, 3, 4,
    5, 6, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 4, 5, 6, 2, 2, 2, 2, 2, 2,
    2, 2, 3, 3, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 1, 1, 1, 2, 2, 4,
    4, 4, 4, 2, 3, 3, 4, 5, 6, 2,
    2, 4, 4, 4, 4, 2, 3, 3, 4, 5,
    6, 2, 2, 4, 4, 4, 4, 2, 3, 3,
    4, 5, 6, 3, 4, 5, 5, 3, 3, 5,
    5, 2, 2, 2, 4, 4, 4, 4, 2, 3,
    3, 4, 5, 6, 2
};


/* Holds the static "rrr" value for instructions only defining "oommm". 
 * if not applicable set to zero. */
unsigned char static_rrr[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    5, 5, 5, 5, 4, 4, 4, 4, 6, 6,
    6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 6, 6, 6, 6, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 4, 4, 2, 2, 2, 2, 3, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    7, 7, 7, 7, 0
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
    HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // add reg8, imm8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // add reg16, imm16
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
    HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // xor reg8, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // xor reg16, reg16
    HAS_OORRRMMM | HAS_REG_USE_RIGHT, // xor mem, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_RIGHT, // xor mem, reg16
    HAS_OORRRMMM | HAS_REG_USE_LEFT, // xor reg8, mem
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT, // xor reg16, mem
    HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // xor al, imm8
    USE_W | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // xor ax, imm16
    HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // xor reg8, imm8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // xor reg16, imm16
    HAS_OOMMM | HAS_IMM_USE_RIGHT, // xor mem, imm8
    USE_W | HAS_OOMMM | HAS_IMM_USE_RIGHT, // xor mem, imm16
    HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // or reg8, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // or reg16, reg16
    HAS_OORRRMMM | HAS_REG_USE_RIGHT, // or mem, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_RIGHT, // or mem, reg16
    HAS_OORRRMMM | HAS_REG_USE_LEFT, // or reg8, mem
    USE_W | HAS_OORRRMMM | HAS_REG_USE_RIGHT, // or reg16, mem
    HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // or al, imm8,
    USE_W | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // or ax, imm16
    HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // or reg8, imm8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // or reg16, imm16
    HAS_OOMMM | HAS_IMM_USE_RIGHT, // or mem, imm8
    USE_W | HAS_OOMMM | HAS_IMM_USE_RIGHT, // or mem, imm16
    HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // and reg8, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // and reg16, reg16
    HAS_OORRRMMM | HAS_REG_USE_RIGHT, // and mem, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_RIGHT, // and mem, reg16
    HAS_OORRRMMM | HAS_REG_USE_LEFT, // and reg8, mem
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT, // and reg16, mem
    HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // and al, imm8,
    USE_W | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // and ax, imm16
    HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // and reg8, imm8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // and reg16, imm16
    HAS_OOMMM | HAS_IMM_USE_RIGHT, // and mem, imm8
    USE_W | HAS_OOMMM | HAS_IMM_USE_RIGHT, // and mem, imm16
    HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // rcl reg8, imm8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // rcl reg16, imm8
    HAS_OOMMM | HAS_IMM_USE_RIGHT, // rcl byte mem, imm8
    USE_W | HAS_OOMMM | HAS_IMM_USE_RIGHT, // rcl word mem, imm16
    HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // rcr reg8, imm8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // rcr reg16, imm8
    HAS_OOMMM | HAS_IMM_USE_RIGHT, // rcr mem, imm8
    USE_W | HAS_OOMMM | HAS_IMM_USE_RIGHT, // rcr mem, imm8
    HAS_IMM_USE_LEFT, // int imm8
    HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // cmp reg8, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT | HAS_REG_USE_RIGHT, // cmp reg16, reg16
    HAS_OORRRMMM | HAS_REG_USE_RIGHT, // cmp mem, reg8
    USE_W | HAS_OORRRMMM | HAS_REG_USE_RIGHT, // cmp mem, reg16
    HAS_OORRRMMM | HAS_REG_USE_LEFT, // cmp reg8, mem
    USE_W | HAS_OORRRMMM | HAS_REG_USE_RIGHT, // cmp reg16, mem
    HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // cmp al, imm8
    USE_W | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // cmp ax, imm16
    HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // cmp reg8, imm8
    USE_W | HAS_OOMMM | HAS_REG_USE_LEFT | HAS_IMM_USE_RIGHT, // cmp reg16, imm16
    HAS_OOMMM | HAS_IMM_USE_RIGHT, // cmp mem, imm8
    USE_W | HAS_OOMMM | HAS_IMM_USE_RIGHT, // cmp mem, imm16
    USE_W | HAS_OORRRMMM | HAS_REG_USE_LEFT, // lea reg16, mem
};

struct ins_syntax_def ins_syntax[] = {
    "mov", MOV_REG_TO_REG_W0, REG8_REG8,
    "mov", MOV_REG_TO_REG_W1, REG16_REG16,
    "mov", MOV_IMM_TO_REG_W0, REG8_IMM8,
    "mov", MOV_IMM_TO_REG_W1, REG16_IMM16,
    "mov", MOV_IMM_TO_MEM_W0, MEM16_IMM8,
    "mov", MOV_IMM_TO_MEM_W1, MEM16_IMM16,
    "mov", MOV_MEM_TO_REG_W0, REG8_MEM16,
    "mov", MOV_MEM_TO_REG_W1, REG16_MEM16,
    "mov", MOV_REG_TO_MEM_W0, MEM16_REG8,
    "mov", MOV_REG_TO_MEM_W1, MEM16_REG16,
    "add", ADD_REG_WITH_REG_W0, REG8_REG8,
    "add", ADD_REG_WITH_REG_W1, REG16_REG16,
    "add", ADD_MEM_WITH_REG_W0, MEM16_REG8,
    "add", ADD_MEM_WITH_REG_W1, MEM16_REG16,
    "add", ADD_REG_WITH_MEM_W0, REG8_MEM16,
    "add", ADD_REG_WITH_MEM_W1, REG16_MEM16,
    "add", ADD_ACC_WITH_IMM_W0, AL_IMM8,
    "add", ADD_ACC_WITH_IMM_W1, AX_IMM16,
    "add", ADD_REG_WITH_IMM_W0, REG8_IMM8,
    "add", ADD_REG_WITH_IMM_W1, REG16_IMM16,
    "add", ADD_MEM_WITH_IMM_W0, MEM16_IMM8,
    "add", ADD_MEM_WITH_IMM_W1, MEM16_IMM16,
    "sub", SUB_REG_WITH_REG_W0, REG8_REG8,
    "sub", SUB_REG_WITH_REG_W1, REG16_REG16,
    "sub", SUB_MEM_WITH_REG_W0, MEM16_REG8,
    "sub", SUB_MEM_WITH_REG_W1, MEM16_REG16,
    "sub", SUB_REG_WITH_MEM_W0, REG8_MEM16,
    "sub", SUB_REG_WITH_MEM_W1, REG16_MEM16,
    "sub", SUB_ACC_WITH_IMM_W0, AL_IMM8,
    "sub", SUB_ACC_WITH_IMM_W1, AX_IMM16,
    "sub", SUB_REG_WITH_IMM_W0, REG8_IMM8,
    "sub", SUB_REG_WITH_IMM_W1, REG16_IMM16,
    "sub", SUB_MEM_WITH_IMM_W0, MEM16_IMM8,
    "sub", SUB_MEM_WITH_IMM_W1, MEM16_IMM16,
    "mul", MUL_WITH_REG_W0, REG8_ALONE,
    "mul", MUL_WITH_REG_W1, REG16_ALONE,
    "mul", MUL_WITH_MEM_W0, MEM8_ALONE,
    "mul", MUL_WITH_MEM_W1, MEM16_ALONE,
    "div", DIV_WITH_REG_W0, REG8_ALONE,
    "div", DIV_WITH_REG_W1, REG16_ALONE,
    "div", DIV_WITH_MEM_W0, MEM8_ALONE,
    "div", DIV_WITH_MEM_W1, MEM16_ALONE,
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
    "ret", RET, ALONE_ALONE,
    "xor", XOR_REG_WITH_REG_W0, REG8_REG8,
    "xor", XOR_REG_WITH_REG_W1, REG16_REG16,
    "xor", XOR_MEM_WITH_REG_W0, MEM16_REG8,
    "xor", XOR_MEM_WITH_REG_W1, MEM16_REG16,
    "xor", XOR_REG_WITH_MEM_W0, REG8_MEM16,
    "xor", XOR_REG_WITH_MEM_W1, REG16_MEM16,
    "xor", XOR_ACC_WITH_IMM_W0, AL_IMM8,
    "xor", XOR_ACC_WITH_IMM_W1, AX_IMM16,
    "xor", XOR_REG_WITH_IMM_W0, REG8_IMM8,
    "xor", XOR_REG_WITH_IMM_W1, REG16_IMM16,
    "xor", XOR_MEM_WITH_IMM_W0, MEM16_IMM8,
    "xor", XOR_MEM_WITH_IMM_W1, MEM16_IMM16,
    "or", OR_REG_WITH_REG_W0, REG8_REG8,
    "or", OR_REG_WITH_REG_W1, REG16_REG16,
    "or", OR_MEM_WITH_REG_W0, MEM16_REG8,
    "or", OR_MEM_WITH_REG_W1, MEM16_REG16,
    "or", OR_REG_WITH_MEM_W0, REG8_MEM16,
    "or", OR_REG_WITH_MEM_W1, REG16_MEM16,
    "or", OR_ACC_WITH_IMM_W0, AL_IMM8,
    "or", OR_ACC_WITH_IMM_W1, AX_IMM16,
    "or", OR_REG_WITH_IMM_W0, REG8_IMM8,
    "or", OR_REG_WITH_IMM_W1, REG16_IMM16,
    "or", OR_MEM_WITH_IMM_W0, MEM16_IMM8,
    "or", OR_MEM_WITH_IMM_W1, MEM16_IMM16,
    "and", AND_REG_WITH_REG_W0, REG8_REG8,
    "and", AND_REG_WITH_REG_W1, REG16_REG16,
    "and", AND_MEM_WITH_REG_W0, MEM16_REG8,
    "and", AND_MEM_WITH_REG_W1, MEM16_REG16,
    "and", AND_REG_WITH_MEM_W0, REG8_MEM16,
    "and", AND_REG_WITH_MEM_W1, REG16_MEM16,
    "and", AND_ACC_WITH_IMM_W0, AL_IMM8,
    "and", AND_ACC_WITH_IMM_W1, AX_IMM16,
    "and", AND_REG_WITH_IMM_W0, REG8_IMM8,
    "and", AND_REG_WITH_IMM_W1, REG16_IMM16,
    "and", AND_MEM_WITH_IMM_W0, MEM16_IMM8,
    "and", AND_MEM_WITH_IMM_W1, MEM16_IMM16,
    "rcl", RCL_REG_WITH_IMM8_W0, REG8_IMM8,
    "rcl", RCL_REG_WITH_IMM8_W1, REG16_IMM8,
    "rcl", RCL_MEM_WITH_IMM8_W0, MEM16_IMM8,
    "rcl", RCL_MEM_WITH_IMM8_W1, MEM16_IMM8,
    "rcr", RCR_REG_WITH_IMM8_W0, REG8_IMM8,
    "rcr", RCR_REG_WITH_IMM8_W1, REG16_IMM8,
    "rcr", RCR_MEM_WITH_IMM8_W0, MEM16_IMM8,
    "rcr", RCR_MEM_WITH_IMM8_W1, MEM16_IMM8,
    "int", INT_IMM8, IMM8_ALONE,
    "cmp", CMP_REG_WITH_REG_W0, REG8_REG8,
    "cmp", CMP_REG_WITH_REG_W1, REG16_REG16,
    "cmp", CMP_MEM_WITH_REG_W0, MEM16_REG8,
    "cmp", CMP_MEM_WITH_REG_W1, MEM16_REG16,
    "cmp", CMP_REG_WITH_MEM_W0, REG8_MEM16,
    "cmp", CMP_REG_WITH_MEM_W1, REG16_MEM16,
    "cmp", CMP_ACC_WITH_IMM_W0, AL_IMM8,
    "cmp", CMP_ACC_WITH_IMM_W1, AX_IMM16,
    "cmp", CMP_REG_WITH_IMM_W0, REG8_IMM8,
    "cmp", CMP_REG_WITH_IMM_W1, REG16_IMM16,
    "cmp", CMP_MEM_WITH_IMM_W0, MEM16_IMM8,
    "cmp", CMP_MEM_WITH_IMM_W1, MEM16_IMM16,
    "lea", LEA_REGWORD_MEM, REG16_MEM16
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
    Assembler::addKeyword("db");
    Assembler::addKeyword("dw");
    Assembler::addKeyword("rb");

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
    Assembler::addInstruction("rcl");
    Assembler::addInstruction("rcr");
    Assembler::addInstruction("cmp");

    this->left = NULL;
    this->right = NULL;
    this->segment = NULL;
    this->cur_offset = 0;

    // Placeholder branch so programmer does not need to check if operand is NULL constantly.
    this->zero_operand_branch = std::shared_ptr<OperandBranch>(new OperandBranch(getCompiler(), NULL));

#ifdef TEST_MODE
    this->cur_ins_sizes = 0;
#endif

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
    return std::shared_ptr<InstructionBranch>(new InstructionBranch(getCompiler(), this->segment_branch));
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
    else if (is_next_extern())
    {
        parse_extern();
    }
    else if (is_next_data())
    {
        parse_data();
    }
    else if (is_next_newline())
    {
        parse_newline();
    }
    else
    {
        peek();
        throw AssemblerException(
                                 "void Assembler8086::parse_part():  unexpected token \"" + getpeekTokenType() +
                                 ": " + getpeekTokenValue() +
                                 "\" on line " + std::to_string(getpeekToken()->getPosition().line_no) +
                                 " this instruction or syntax may not be implemented."
                                 );
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

std::vector<std::shared_ptr<Branch>> Assembler8086::get_register_branches_from_exp(std::shared_ptr<Branch> branch)
{
    std::vector<std::shared_ptr < Branch>> result;
    if (branch->getType() != "E")
    {
        if (branch->getType() == "register")
        {
            result.push_back(branch);
        }
    }
    else
    {
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(branch);
        e_branch->iterate_expressions([&](std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) -> void
        {
            if (left_branch->getType() == "register")
            {
                result.push_back(left_branch);
            }

            if (right_branch->getType() == "register")
            {
                result.push_back(right_branch);
            }
        });
    }

    return result;
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
    std::vector<std::shared_ptr<Branch>> register_branches = get_register_branches_from_exp(r_exp);
    std::shared_ptr<Branch> identifier_branch = get_identifier_branch_from_exp(r_exp);

    std::shared_ptr<Branch> first_reg_branch = NULL;
    std::shared_ptr<Branch> second_reg_branch = NULL;
    
    // We clone because the framework does not allow the child to have two parents.
    if (number_branch != NULL)
        number_branch = number_branch->clone();
    
    if (register_branches.size() > 2)
    {
         throw Exception("Maximum 2 registers are allowed to be present", "void Assembler8086::handle_operand_exp(std::shared_ptr<OperandBranch> operand_branch)");
    }
    if (register_branches.size() > 0)
    {
        first_reg_branch = register_branches[0]->clone();
        if (register_branches.size() == 2)
        {
            second_reg_branch = register_branches[1]->clone();
        }
    }
    if (identifier_branch != NULL)
        identifier_branch = identifier_branch->clone();

    operand_branch->setNumberBranch(number_branch);
    operand_branch->setFirstRegisterBranch(first_reg_branch);
    operand_branch->setSecondRegisterBranch(second_reg_branch);
    operand_branch->setIdentifierBranch(identifier_branch);
}

void Assembler8086::parse_operand(OPERAND_DATA_SIZE data_size)
{
    std::shared_ptr<OperandBranch> operand_branch = std::shared_ptr<OperandBranch>(new OperandBranch(getCompiler(), this->segment_branch));
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
    // Shift and pop the "segment" keyword
    shift_pop();

    // Next up is the segment name
    shift_pop();
    std::shared_ptr<Branch> segment_name_branch = Assembler::getPoppedBranch();

    std::shared_ptr<SegmentBranch> segment_root = std::shared_ptr<SegmentBranch>(new SegmentBranch(getCompiler()));
    segment_root->setSegmentNameBranch(segment_name_branch);

    // Create the contents branch and add it to the segment branch
    std::shared_ptr<Branch> contents_branch = std::shared_ptr<Branch>(new Branch("CONTENTS", ""));
    segment_root->setContentsBranch(contents_branch);

    // Lets save the register branch in memory for later use throughout the parsing.
    this->segment_branch = segment_root;

    while (hasTokens())
    {
        if (is_next_segment())
        {
            /* We should stop now as we are at another segment, segments are independent from other segments*/
            break;
        }

        parse_part();
        pop_branch();
        // We don't want to add new lines as children, its a bit hacky struggling to think of a better solution at the moment
        if (getPoppedBranchType() != "new_line")
        {
            // Add the branch to the segment contents branch
            contents_branch->addChild(getPoppedBranch());
        }


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
    std::shared_ptr<LabelBranch> label_branch = std::shared_ptr<LabelBranch>(new LabelBranch(getCompiler(), this->segment_branch));
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
        // We don't want to add new lines, its a bit hacky I can't think of a better solution at the moment
        if (getPoppedBranchType() != "new_line")
        {
            // Add the branch to the label contents branch
            label_contents_branch->addChild(getPoppedBranch());
        }

    }

    push_branch(label_branch);
}

void Assembler8086::parse_ins()
{
    std::shared_ptr<Branch> name_branch = NULL;
    std::shared_ptr<OperandBranch> dest_op = NULL;
    std::shared_ptr<OperandBranch> source_op = NULL;

    OPERAND_DATA_SIZE def_data_size = OPERAND_DATA_SIZE_UNKNOWN;

    // Shift and pop the instruction
    shift_pop();
    name_branch = getPoppedBranch();

    // Do we have an expression
    peek();
    if (is_next_valid_operand())
    {
        // Next will be the left operand
        parse_operand(def_data_size);
        // Pop it off
        pop_branch();
        dest_op = std::dynamic_pointer_cast<OperandBranch>(getPoppedBranch());

        // Get the data size for the recently parsed operand, we will set it ready for the second operand (if any)
        def_data_size = dest_op->getDataSize();

        // Do we have a second operand?
        peek();
        if (is_peek_symbol(","))
        {
            // Now we need to shift and pop off the comma ","
            shift_pop();

            // Finally a final expression which will be the second operand
            parse_operand(def_data_size);

            // Pop it off
            pop_branch();
            source_op = std::dynamic_pointer_cast<OperandBranch>(getPoppedBranch());
        }
    }

    // Put it all together
    std::shared_ptr<InstructionBranch> ins_branch = std::shared_ptr<InstructionBranch>(new InstructionBranch(getCompiler(), this->segment_branch));
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
    std::shared_ptr<GlobalBranch> global_branch = std::shared_ptr<GlobalBranch>(new GlobalBranch(getCompiler(), this->segment_branch));

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

void Assembler8086::parse_extern()
{
    // Shift and pop the extern keyword
    shift_pop();

    peek();
    if (!is_peek_type("identifier"))
    {
        throw Exception("void Assembler8086::parse_extern(): expecting an identifier but none was provided");
    }

    // Ok lets shift and pop the extern name
    shift_pop();

    std::shared_ptr<Branch> extern_name_branch = getPoppedBranch();
    std::shared_ptr<ExternBranch> extern_branch = std::shared_ptr<ExternBranch>(new ExternBranch(getCompiler()));
    extern_branch->setNameBranch(extern_name_branch);

    // The extern branch has been constructed so lets push it to the stack
    push_branch(extern_branch);

}

void Assembler8086::parse_data(DATA_BRANCH_TYPE data_branch_type)
{
    std::shared_ptr<DataBranch> data_branch = std::shared_ptr<DataBranch>(new DataBranch(getCompiler(), this->segment_branch));

    // Has a custom data type been specified? If so then we do not need to look for "db" or "dw"
    if (data_branch_type == -1)
    {
        // Shift and pop the data keyword
        shift_pop();
        std::string data_keyword_value = getPoppedBranchValue();
        if (data_keyword_value == "db")
        {
            data_branch_type = DATA_BRANCH_TYPE_DATA_BYTE;
        }
        else if (data_keyword_value == "dw")
        {
            data_branch_type = DATA_BRANCH_TYPE_DATA_WORD;
        }
        else if (data_keyword_value == "rb")
        {
            data_branch_type = DATA_BRANCH_TYPE_DATA_RESERVE_BYTE;
        }
    }

    data_branch->setDataBranchType(data_branch_type);

    peek();
    if (is_peek_type("number") || is_peek_type("string"))
    {
        shift_pop();
        data_branch->setData(getPoppedBranch());
    }
    else
    {
        throw Exception("void Assembler8086::parse_data(): unexpected token type: "
                        + Assembler::getpeekTokenType() + " of value \""
                        + Assembler::getpeekTokenValue() + "\" + expecting either a \"number\" or \"string\" type.");
    }

    // Is there more data coming up?
    peek();
    if (is_peek_symbol(","))
    {
        //  Yes we have a comma shift it off and recall this method
        shift_pop();
        parse_data(data_branch_type);
        // Pop off the DATA branch
        pop_branch();
        // Add it to our data branch
        data_branch->setNextDataBranch(std::dynamic_pointer_cast<DataBranch>(getPoppedBranch()));
    }

    // Push the result
    push_branch(data_branch);
}

void Assembler8086::parse_newline()
{
    // Shift the new line we still need it on the tree because each parse is required to push a branch
    shift();
}

bool Assembler8086::is_next_valid_operand()
{
    return (is_peek_type("identifier")
            || is_peek_type("number")
            || is_peek_type("register")
            || is_peek_symbol("[")
            || (is_peek_type("keyword") && (is_peek_value("byte") || is_peek_value("word")))
            );
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

bool Assembler8086::is_next_extern()
{
    peek();
    return is_peek_keyword("extern");
}

bool Assembler8086::is_next_data()
{
    peek();
    return is_peek_keyword("db") || is_peek_keyword("dw") || is_peek_keyword("rb");
}

bool Assembler8086::is_next_newline()
{
    peek();
    return is_peek_type("new_line");
}

void Assembler8086::generate()
{
#ifdef TEST_MODE
    std::cout << "Test mode is enabled, tests will be preformed." << std::endl;
#endif

    // Calculates instruction offsets.
    assembler_pass_1();
    // Handles the accessing of labels and if there is a problem then it will store it for assessment in pass 3
    assembler_pass_2();
    // Figures out if labels are not out of range for particular instructions
    assembler_pass_3();
    // Generates the instructions into machine code.
    assembler_pass_4();

#ifdef TEST_MODE
    // Test mode is defined so lets just check that the size of the stream matches the stream length
    if (this->sstream->getSize() == this->cur_ins_sizes)
    {
        std::cout << "All instruction sizes match the size of the stream" << std::endl;
    }
    else
    {
        std::cout << "The instructions summed ins_sizes do not match the size of the stream. If this stream contains only instructions and only one segment then the ins_sizes array is invalid" << std::endl;
    }
#endif

}

void Assembler8086::push_branch(std::shared_ptr<Branch> branch)
{
    Assembler::push_branch(branch);

    // Are we pushing an offsetable branch?
    std::shared_ptr<OffsetableBranch> offsetable_branch = std::dynamic_pointer_cast<OffsetableBranch>(branch);
    if (offsetable_branch != NULL)
    {
        // Yes we did lets set the previous offsetable branches next offsetable branch to us
        if (this->last_offsetable_branch != NULL)
        {
            this->last_offsetable_branch->setNextOffsetableBranch(offsetable_branch);
        }

        this->last_offsetable_branch = offsetable_branch;
    }
}

std::shared_ptr<MustFitTable> Assembler8086::get_must_fit_table_for_label(std::string label_name)
{
    std::shared_ptr<LabelBranch> label_branch = get_label_branch(label_name);
    if (label_branch != NULL)
    {
        return label_branch->getMustFitTable();
    }

    return NULL;
}

void Assembler8086::assembler_pass_1()
{
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        if (branch->getType() == "SEGMENT")
        {
            pass_1_segment(std::dynamic_pointer_cast<SegmentBranch>(branch));
        }
        else
        {
            throw new Exception("branch requires a segment.", "void Assembler8086::generate()");
        }
    }
}

void Assembler8086::pass_1_segment(std::shared_ptr<SegmentBranch> segment_branch)
{
    // Reset the current offset ready for the next segment
    this->cur_offset = 0;
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
        // Lets calculate the operand sizes for this instruction
        calculate_operand_sizes_for_instruction(ins_branch);

        ins_branch->setOffset(this->cur_offset);
        int size = get_instruction_size(std::dynamic_pointer_cast<InstructionBranch>(branch));
        ins_branch->setSize(size);
        this->cur_offset += size;
    }
    else if (branch->getType() == "GLOBAL")
    {

    }
    else if (branch->getType() == "EXTERN")
    {
        std::shared_ptr<ExternBranch> extern_branch = std::dynamic_pointer_cast<ExternBranch>(branch);
        getObjectFormat()->registerExternalReference(extern_branch->getNameBranch()->getValue());
    }
    else if (branch->getType() == "DATA")
    {
        std::shared_ptr<DataBranch> data_branch = std::dynamic_pointer_cast<DataBranch>(branch);
        std::shared_ptr<Branch> d_branch = data_branch->getData();
        DATA_BRANCH_TYPE data_branch_type = data_branch->getDataBranchType();
        int size;
        if (data_branch_type == DATA_BRANCH_TYPE_DATA_RESERVE_BYTE)
        {
            data_branch->setOffset(this->cur_offset);
            // We are reserving bytes so all we really want is a number
            if (d_branch->getType() != "number")
            {
                throw Exception("Expecting a \"number\" for assembler data \"rb\" keyword but a \"" + d_branch->getType() + "\" keyword was provided");
            }
            int total_to_reserve = std::stoi(d_branch->getValue());
            size = total_to_reserve;
            this->cur_offset += size;
        }
        else
        {
            do
            {
                data_branch->setOffset(this->cur_offset);
                // We need to get the size of the data branches data
                size = 0;
                d_branch = data_branch->getData();
                data_branch_type = data_branch->getDataBranchType();
                if (d_branch->getType() == "string")
                {
                    size = d_branch->getValue().length();
                    if (data_branch_type == DATA_BRANCH_TYPE_DATA_WORD)
                    {
                        // Maybe this exception is more appropriate somewhere else?
                        throw Exception("void Assembler8086::pass_1_part(std::shared_ptr<Branch> branch): wide strings are not supported by this assembler.");
                    }
                }
                else
                {
                    // Ok this must be a number, so just set the size based on weather its a byte or a word
                    size = (data_branch_type == DATA_BRANCH_TYPE_DATA_BYTE ? 1 : 2);
                }

                // We have successfully calculated the size lets adjust the offset
                this->cur_offset += size;

                // Do we have nested data branches?
                if (data_branch->hasNextDataBranch())
                {
                    data_branch = data_branch->getNextDataBranch();
                }
                else
                {
                    break;
                }
            }
            while (true);
        }
    }
    else
    {
        throw AssemblerException("void Assembler8086::pass_1_part(std::shared_ptr<Branch> branch): "
                                 "unsupported branch type of type \"" + branch->getType() + "\" was provided");
    }
}

void Assembler8086::assembler_pass_2()
{
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        if (branch->getType() == "SEGMENT")
        {
            pass_2_segment(std::dynamic_pointer_cast<SegmentBranch>(branch));
        }
    }
}

void Assembler8086::pass_2_segment(std::shared_ptr<SegmentBranch> segment_branch)
{
    // Switch to the segment
    switch_to_segment(segment_branch->getSegmentNameBranch()->getValue());
    // Now we need to pass through the children
    for (std::shared_ptr<Branch> child : segment_branch->getContentsBranch()->getChildren())
    {
        pass_2_part(child);
    }
}

void Assembler8086::pass_2_part(std::shared_ptr<Branch> branch)
{
    std::string type = branch->getType();
    if (type == "INSTRUCTION")
    {
        std::shared_ptr<InstructionBranch> ins_branch = std::dynamic_pointer_cast<InstructionBranch>(branch);
        /* An operand may have specified a label that is too far in offset for the given instruction
         * We register possible scenarios like this so that they can be resolved later on if required.*/
        add_must_fits_if_required(ins_branch);
    }

}

void Assembler8086::assembler_pass_3()
{
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        if (branch->getType() == "SEGMENT")
        {
            pass_3_segment(std::dynamic_pointer_cast<SegmentBranch>(branch));
        }
    }
}

void Assembler8086::pass_3_segment(std::shared_ptr<SegmentBranch> segment_branch)
{
    // Switch to the segment
    switch_to_segment(segment_branch->getSegmentNameBranch()->getValue());
    // Now we need to pass through the children
    for (std::shared_ptr<Branch> child : segment_branch->getContentsBranch()->getChildren())
    {
        pass_3_part(child);
    }
}

void Assembler8086::pass_3_part(std::shared_ptr<Branch> branch)
{
    std::string type = branch->getType();
    if (type == "LABEL")
    {
        std::shared_ptr<LabelBranch> label_branch = std::dynamic_pointer_cast<LabelBranch>(branch);
        // Ok we need to register a global reference (if any)
        register_global_reference_if_any(label_branch);
        // Ok we need to handle the must fit
        handle_mustfits_for_label_branch(label_branch);
    }

}

void Assembler8086::handle_mustfits_for_label_branch(std::shared_ptr<LabelBranch> label_branch)
{
    std::shared_ptr<MustFitTable> must_fit_table = label_branch->getMustFitTable();

    // Lets check if any must ifts are required
    if (must_fit_table->hasMustFits())
    {
        for (struct MUST_FIT must_fit : must_fit_table->getMustFits())
        {
            std::shared_ptr<InstructionBranch> ins_branch = must_fit.ins_branch;
            int our_pos = label_branch->getOffset();
            int ins_pos = ins_branch->getOffset();
            int offset = our_pos - ins_pos;
            if (must_fit.must_fit == MUST_FIT_8_BIT_SIGNED)
            {

                if (offset > 128
                        || offset < -128)
                {
                    // Offset is above or below 128 so it will not fit into an 8 bit singed integer
                    // Lets just throw an exception here then since the programmer did something they shouldn't have done
                    // and this assembler currently does not fix these problems automatically.
                    throw Exception("void Assembler8086::handle_mustfits_for_label_branch(std::shared_ptr<LabelBranch> label_branch): offset boundaries breached for instruction " + ins_branch->getInstructionNameBranch()->getValue());
                }
            }
        }
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
    std::string left_reg_first_value = "";
    std::string left_reg_second_value = "";
    std::string right_reg_first_value = "";
    std::string right_reg_second_value = "";

    if (ins_branch->hasLeftBranch())
    {
        left = ins_branch->getLeftBranch();
        if (left->hasFirstRegisterBranch())
        {
            left_reg_first = left->getFirstRegisterBranch();
            left_reg_first_value = left_reg_first->getValue();
        }
        
        if (left->hasSecondRegisterBranch())
        {
            left_reg_second = left->getSecondRegisterBranch();
            left_reg_second_value = left_reg_second->getValue();
        }
    }

    if (ins_branch->hasRightBranch())
    {
        right = ins_branch->getRightBranch();
        if (right->hasFirstRegisterBranch())
        {
            right_reg_first = right->getFirstRegisterBranch();
            right_reg_first_value = right_reg_first->getValue();
        }
        
        if (right->hasSecondRegisterBranch())
        {
            right_reg_second = right->getSecondRegisterBranch();
            right_reg_second_value = right_reg_second->getValue();
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
                    *oo = DISPLACEMENT_16BIT_FOLLOW;
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
            *mmm = get_mmm(left_reg_first_value, left_reg_second_value);
        }
        else
        {
            if (*oo == DISPLACEMENT_IF_MMM_110 ||
                    *oo == DISPLACEMENT_8BIT_FOLLOW ||
                    *oo == DISPLACEMENT_16BIT_FOLLOW)
            {
                *rrr = get_reg(left_reg_first_value);
            }
            else
            {
                *mmm = get_reg(left_reg_first_value);
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
                *mmm = get_mmm(right_reg_first_value, right_reg_second_value);
            }
            else
            {
                *rrr = get_mmm(right_reg_first_value, right_reg_second_value);
            }
        }
        else
        {
            *rrr = get_reg(right_reg_first_value);
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
    this->segment_branches.push_back(segment_branch);
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

void Assembler8086::assembler_pass_4()
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
        selected_reg = ins_branch->getLeftBranch()->getFirstRegisterBranch();
    }
    else if (info & HAS_REG_USE_RIGHT)
    {
        selected_reg = ins_branch->getRightBranch()->getFirstRegisterBranch();
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
        write_modrm_offset(oo, mmm, ins_branch, selected_branch);
    }
}

void Assembler8086::gen_imm(INSTRUCTION_INFO info, std::shared_ptr<InstructionBranch> ins_branch)
{
    std::shared_ptr<OperandBranch> selected_operand = NULL;

    // Get current address we are on
    int cur_address = sstream->getPosition();
    FIXUP_LENGTH length;

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
        length = FIXUP_16BIT;
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
        length = FIXUP_8BIT;
    }

    // Register a fixup if we need to
    register_fixup_if_required(cur_address, length, ins_branch, selected_operand);
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
    else if (branch->getType() == "DATA")
    {
        generate_data(std::dynamic_pointer_cast<DataBranch>(branch));
    }


}

void Assembler8086::generate_instruction(std::shared_ptr<InstructionBranch> instruction_branch)
{
    std::string ins_name = instruction_branch->getInstructionNameBranch()->getValue();
    INSTRUCTION_TYPE ins_type = get_instruction_type(instruction_branch);
    cur_ins_type = ins_type;

    int opcode = ins_map[ins_type];
    INSTRUCTION_INFO info = ins_info[ins_type];
#ifdef TEST_MODE
    this->cur_ins_sizes += get_instruction_size(instruction_branch);
#endif
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

void Assembler8086::generate_data(std::shared_ptr<DataBranch> data_branch)
{
    std::shared_ptr<Branch> d_branch = data_branch->getData();
    DATA_BRANCH_TYPE data_branch_type = data_branch->getDataBranchType();
    if (data_branch_type == DATA_BRANCH_TYPE_DATA_BYTE)
    {
        if (d_branch->getType() == "string")
        {
            // This data is a string so write it and do not write the NULL terminator.
            this->sstream->writeStr(d_branch->getValue(), false);
        }
        else
        {
            // This data is a byte so write it.
            this->sstream->write8(std::stoi(d_branch->getValue()));
        }
    }
    else if (data_branch_type == DATA_BRANCH_TYPE_DATA_RESERVE_BYTE)
    {
        // Ok we are reserving bytes here so we should just write NULL's for how many bytes their are to reserve
        int total_to_null = std::stoi(d_branch->getValue());
        for (int i = 0; i < total_to_null; i++)
        {
            this->sstream->write8(0);
        }
    }
    else
    {
        // This data is a word so write it.
        this->sstream->write16(std::stoi(d_branch->getValue()));
    }

    // Do we have nested data to generate?
    if (data_branch->hasNextDataBranch())
    {
        generate_part(data_branch->getNextDataBranch());
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

bool Assembler8086::has_label_branch(std::string label_name)
{
    for (std::shared_ptr<SegmentBranch> i_segment : segment_branches)
    {
        for (std::shared_ptr<Branch> child : i_segment->getContentsBranch()->getChildren())
        {
            if (child->getType() == "LABEL")
            {
                std::shared_ptr<LabelBranch> lbl_branch = std::dynamic_pointer_cast<LabelBranch>(child);
                if (lbl_branch->getLabelNameBranch()->getValue() == label_name)
                    return true;
            }
        }
    }

    return false;
}

bool Assembler8086::has_global(std::string global_name)
{
    for (std::shared_ptr<SegmentBranch> i_segment : segment_branches)
    {
        for (std::shared_ptr<Branch> child : i_segment->getContentsBranch()->getChildren())
        {
            if (child->getType() == "GLOBAL")
            {
                std::shared_ptr<GlobalBranch> global_branch = std::dynamic_pointer_cast<GlobalBranch>(child);
                if (global_branch->getLabelNameBranch()->getValue() == global_name)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Assembler8086::has_extern(std::string extern_name)
{
    for (std::shared_ptr<SegmentBranch> i_segment : segment_branches)
    {
        for (std::shared_ptr<Branch> child : i_segment->getContentsBranch()->getChildren())
        {
            if (child->getType() == "EXTERN")
            {
                std::shared_ptr<ExternBranch> extern_branch = std::dynamic_pointer_cast<ExternBranch>(child);
                if (extern_branch->getNameBranch()->getValue() == extern_name)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Assembler8086::has_segment(std::string segment_name)
{
    for (std::shared_ptr<SegmentBranch> i_segment : segment_branches)
    {
        if (i_segment->getSegmentNameBranch()->getValue() == segment_name)
            return true;
    }

    return false;
}

IDENTIFIER_TYPE Assembler8086::get_identifier_type(std::string iden_name)
{
    IDENTIFIER_TYPE iden_type = -1;
    // Do we have a label?
    if (has_label_branch(iden_name))
    {
        iden_type = IDENTIFIER_TYPE_LABEL;
    }
    else if (has_extern(iden_name))
    {
        iden_type = IDENTIFIER_TYPE_EXTERN;
    }
    else if (has_segment(iden_name))
    {
        iden_type = IDENTIFIER_TYPE_SEGMENT;
    }
    else
    {
        throw Exception("IDENTIFIER_TYPE Assembler8086::get_identifier_type(std::string iden_name): identifier type unknown");
    }

    return iden_type;
}

int Assembler8086::get_label_offset(std::string label_name, std::shared_ptr<InstructionBranch> ins_branch, bool short_or_near_possible)
{
    int value;
    std::shared_ptr<LabelBranch> label_branch = get_label_branch(label_name);
    int lbl_offset = label_branch->getOffset();

    // We should only calculate short or near for labels on the same segment as the instruction, otherwise we will give them a position relative to the segment
    if (short_or_near_possible
            && ins_branch->getSegmentBranch() == label_branch->getSegmentBranch())
    {
        // Ok its possible to deal with this as a short so lets do that, this is valid with things such as short jumps.  
        int ins_offset = ins_branch->getOffset();
        int ins_size = ins_branch->getSize();
        int abs = lbl_offset - ins_offset - ins_size;
        value = abs;
    }
    else
    {
        value = lbl_offset;
    }

    return value;
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
        std::string iden_value = branch->getIdentifierBranch()->getValue();
        IDENTIFIER_TYPE iden_type = get_identifier_type(iden_value);
        switch (iden_type)
        {
        case IDENTIFIER_TYPE_LABEL:
            value += get_label_offset(iden_value, ins_branch, short_or_near_possible);
            break;
        case IDENTIFIER_TYPE_SEGMENT:
            // Nothing to do with segments yet
            break;
        }

    }

    return value;
}

std::shared_ptr<VirtualSegment> Assembler8086::get_virtual_segment_for_label(std::string label_name)
{
    std::shared_ptr<LabelBranch> label_branch = get_label_branch(label_name);
    std::string segment_name = label_branch->getSegmentBranch()->getSegmentNameBranch()->getValue();
    return getObjectFormat()->getSegment(segment_name);
}

void Assembler8086::register_global_reference_if_any(std::shared_ptr<LabelBranch> label_branch)
{
    std::string label_name = label_branch->getLabelNameBranch()->getValue();
    if (has_global(label_name))
    {
        getObjectFormat()->registerGlobalReference(this->segment, label_name, label_branch->getOffset());
    }
}

void Assembler8086::register_fixup_if_required(int offset, FIXUP_LENGTH length, std::shared_ptr<InstructionBranch> ins_branch, std::shared_ptr<OperandBranch> branch)
{
    if (!branch->hasIdentifierBranch())
    {
        // Ok no identifier branch so nothing to do just return
        return;
    }

    // Ok lets register this fixup
    std::string iden_value = branch->getIdentifierBranch()->getValue();
    IDENTIFIER_TYPE iden_type = get_identifier_type(iden_value);
    INSTRUCTION_INFO i_info = ins_info[get_instruction_type(ins_branch)];
    FIXUP_TYPE fixup_type;
    if (i_info & SHORT_POSSIBLE
            || i_info & NEAR_POSSIBLE)
    {
        // This instruction uses relative addressing
        fixup_type = FIXUP_TYPE_SELF_RELATIVE;
    }
    else
    {
        fixup_type = FIXUP_TYPE_SEGMENT;
    }

    if (iden_type == IDENTIFIER_TYPE_LABEL)
    {
        // We should only register segment relative fixups for anything other than our target segment
        std::shared_ptr<VirtualSegment> ins_segment = getObjectFormat()->getSegment(ins_branch->getSegmentBranch()->getSegmentNameBranch()->getValue());
        std::shared_ptr<VirtualSegment> target_segment = get_virtual_segment_for_label(iden_value);
        if (ins_segment != target_segment)
        {
            segment->register_fixup_target_segment(fixup_type, target_segment, offset, length);
        }
    }
    else if (iden_type == IDENTIFIER_TYPE_EXTERN)
    {
        segment->register_fixup_target_extern(fixup_type, iden_value, offset, length);
    }
}

void Assembler8086::write_modrm_offset(unsigned char oo, unsigned char mmm, std::shared_ptr<InstructionBranch> ins_branch, std::shared_ptr<OperandBranch> branch)
{
    // Fixups are also registered in this method so that the linker may replace the offset at a later date.

    // Get current address we are on
    int cur_address = sstream->getPosition();
    FIXUP_LENGTH length;
    switch (oo)
    {
    case DISPLACEMENT_IF_MMM_110:
        if (mmm == 0b110)
        {
            write_abs_static16(branch);
            length = FIXUP_16BIT;
        }
        break;
    case DISPLACEMENT_8BIT_FOLLOW:
        write_abs_static8(branch);
        length = FIXUP_8BIT;
        break;
    case DISPLACEMENT_16BIT_FOLLOW:
        write_abs_static16(branch);
        length = FIXUP_16BIT;
        break;
    }

    // Register a fixup if we need to
    register_fixup_if_required(cur_address, length, ins_branch, branch);
}

unsigned char Assembler8086::write_abs_static8(std::shared_ptr<OperandBranch> branch, bool short_possible, std::shared_ptr<InstructionBranch> ins_branch)
{
    unsigned char s = get_static_from_branch(branch, short_possible, ins_branch);
    sstream->write8(s);
    return s;
}

unsigned short Assembler8086::write_abs_static16(std::shared_ptr<OperandBranch> branch, bool near_possible, std::shared_ptr<InstructionBranch> ins_branch)
{
    unsigned short s = get_static_from_branch(branch, near_possible, ins_branch);
    sstream->write16(s);
    return s;
}

std::shared_ptr<LabelBranch> Assembler8086::get_label_branch(std::string label_name)
{
    for (std::shared_ptr<SegmentBranch> i_segment : segment_branches)
    {
        for (std::shared_ptr<Branch> child : i_segment->getContentsBranch()->getChildren())
        {
            if (child->getType() == "LABEL")
            {
                std::shared_ptr<LabelBranch> lbl_branch = std::dynamic_pointer_cast<LabelBranch>(child);
                if (lbl_branch->getLabelNameBranch()->getValue() == label_name)
                    return lbl_branch;
            }
        }
    }

    throw Exception("std::shared_ptr<LabelBranch> Assembler8086::get_label_branch(std::string label_name): the label branch does not exist");
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
        reg_branch = op_branch->getFirstRegisterBranch();
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
            info = IMM16;
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
            info = MEM8;
        }
        else if (op_branch->getDataSize() == OPERAND_DATA_SIZE_WORD)
        {
            info = MEM16;
        }
    }

    return info;
}

SYNTAX_INFO Assembler8086::get_syntax_info(std::shared_ptr<InstructionBranch> instruction_branch, OPERAND_INFO* left_op, OPERAND_INFO* right_op)
{
    OPERAND_INFO left_op_local = ALONE;
    OPERAND_INFO right_op_local = ALONE;
    if (instruction_branch->hasLeftBranch())
    {
        left_op_local = get_operand_info(instruction_branch->getLeftBranch());
    }
    if (instruction_branch->hasRightBranch())
    {
        right_op_local = get_operand_info(instruction_branch->getRightBranch());
    }

    if (left_op != NULL)
    {
        *left_op = left_op_local;
    }

    if (right_op != NULL)
    {
        *right_op = right_op_local;
    }

    return (left_op_local << OPERAND_BIT_SIZE | right_op_local);
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

#ifdef DEBUG_MODE

void Assembler8086::output_syntax_info(SYNTAX_INFO syntax_info)
{
    OPERAND_INFO left = syntax_info >> OPERAND_BIT_SIZE;
    OPERAND_INFO right = syntax_info;

    std::cout << "Syntax info: " << get_operand_info_as_string(left) << ":" << get_operand_info_as_string(right) << std::endl;
}

std::string Assembler8086::get_operand_info_as_string(OPERAND_INFO operand_info)
{
    return std::string(operand_info_str[operand_info]);
}

#endif

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

            // Bit of a hacky way to get the position of a token
            std::shared_ptr<Token> token = std::dynamic_pointer_cast<Token>(instruction_branch->getInstructionNameBranch());
            CharPos char_pos = token->getPosition();
            // There truly is a problem
            throw AssemblerException("The instruction: \"" + instruction_name + "\" on line " + std::to_string(char_pos.line_no) + " does not exist or is not implemented or you are using an illegal syntax");
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
        left_reg_first = left->getFirstRegisterBranch();
        if (right->isOnlyRegister())
        {
            // Register to register assignment "mov reg, reg", 8 bit or 16 bit assignment?
            if (is_reg_16_bit(left_reg_first->getValue()))
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
            if (is_reg_16_bit(left_reg_first->getValue()))
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
            if (is_reg_16_bit(left_reg_first->getValue()))
            {
                return MOV_MEM_TO_REG_W1;
            }
            else
            {
                return MOV_MEM_TO_REG_W0;
            }

        }
    }
    else if (left->isAccessingMemory())
    {
        // Memory assignment.
        if (right->isOnlyRegister())
        {
            right_reg_first = right->getFirstRegisterBranch();
            // mov mem, reg
            if (is_reg_16_bit(right_reg_first->getValue()))
            {
                return MOV_REG_TO_MEM_W1;
            }
            else
            {
                return MOV_REG_TO_MEM_W0;
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
        left_reg_first = left->getFirstRegisterBranch();
        if (right->isOnlyRegister())
        {
            if (is_reg_16_bit(left_reg_first->getValue()))
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
            if (is_reg_16_bit(left_reg_first->getValue()))
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
        right_reg_first = right->getFirstRegisterBranch();
        if (left->isAccessingMemory())
        {
            if (right->isOnlyRegister())
            {
                if (is_reg_16_bit(right_reg_first->getValue()))
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

OPERAND_DATA_SIZE Assembler8086::get_data_size_for_reg(std::string reg)
{
    OPERAND_DATA_SIZE def_data_size = OPERAND_DATA_SIZE_UNKNOWN;

    if (is_reg_16_bit(reg))
    {
        def_data_size = OPERAND_DATA_SIZE_WORD;
    }
    else
    {
        def_data_size = OPERAND_DATA_SIZE_BYTE;
    }

    return def_data_size;
}

OPERAND_DATA_SIZE Assembler8086::get_operand_data_size_for_number(int number)
{
    OPERAND_DATA_SIZE size;
    if (number < 256)
    {
        size = OPERAND_DATA_SIZE_BYTE;
    }
    else
    {
        size = OPERAND_DATA_SIZE_WORD;
    }

    return size;
}

void Assembler8086::calculate_data_size_for_operand(std::shared_ptr<OperandBranch> branch)
{
    std::shared_ptr<InstructionBranch> ins_branch = branch->getInstructionBranch();
    if (branch->getDataSize() != OPERAND_DATA_SIZE_UNKNOWN)
    {
        // The size has already been decided externally nothing we can do here
        return;
    }

    OPERAND_DATA_SIZE size = OPERAND_DATA_SIZE_UNKNOWN;
    if (branch->hasRegisterBranch())
    {
        size = get_data_size_for_reg(branch->getFirstRegisterBranch()->getValue());
    }
    else if (branch->hasIdentifierBranch())
    {
        size = OPERAND_DATA_SIZE_BYTE;
        // Set the data size ready for get_syntax_info
        branch->setDataSize(size);
        SYNTAX_INFO syntax_info = get_syntax_info(ins_branch);
        INSTRUCTION_TYPE type = get_instruction_type_by_name_and_syntax(ins_branch->getInstructionNameBranch()->getValue(), syntax_info);
        if (type == -1)
        {
            /* Illegal instruction so lets set it to a word instead and
             * if there is still a problem it will be caught later on.
             */
            size = OPERAND_DATA_SIZE_WORD;
        }
    }
    else if (branch->hasNumberBranch())
    {
        unsigned int number = std::stoi(branch->getNumberBranch()->getValue());
        if (number < 256)
        {
            // we can fit this in a byte
            size = OPERAND_DATA_SIZE_BYTE;
            // Set the data size ready for get_syntax_info
            branch->setDataSize(size);
            SYNTAX_INFO syntax_info = get_syntax_info(ins_branch);
            INSTRUCTION_TYPE type = get_instruction_type_by_name_and_syntax(ins_branch->getInstructionNameBranch()->getValue(), syntax_info);
            if (type == -1)
            {
                /* Illegal instruction so lets set it to a word instead and
                 * if there is still a problem it will be caught later on.
                 */
                size = OPERAND_DATA_SIZE_WORD;
            }

        }
        else
        {
            // We need to use a word here
            size = OPERAND_DATA_SIZE_WORD;
        }
    }

    branch->setDataSize(size);
}

void Assembler8086::calculate_operand_sizes_for_instruction(std::shared_ptr<InstructionBranch> instruction_branch)
{
    /* Ok we need to calculate the operand sizes for this instruction as they are not yet calculated 
     * the algorithm is as follows.
     */

    std::shared_ptr<OperandBranch> left = instruction_branch->getLeftBranch();
    std::shared_ptr<OperandBranch> right = instruction_branch->getRightBranch();

    SYNTAX_INFO op_syntax_info;
    OPERAND_DATA_SIZE data_size;

    // If there is a register on either operand it should effect the size of the other operand if the size is currently unknown
    if (instruction_branch->hasLeftBranch()
            && left->isOnlyRegister() && left->getDataSize() == OPERAND_DATA_SIZE_UNKNOWN)
    {
        data_size = get_data_size_for_reg(left->getFirstRegisterBranch()->getValue());
        left->setDataSize(data_size);
        if (instruction_branch->hasRightBranch()
                && right->getDataSize() == OPERAND_DATA_SIZE_UNKNOWN && !right->isAccessingMemory())
        {
            // Ok lets set the right to the lefts data size
            right->setDataSize(data_size);
        }
    }

    // I am repeating code here, I should clean it up at a later date, cant thing of an appropriate method name at the moment.
    if (instruction_branch->hasRightBranch()
            && right->isOnlyRegister() && right->getDataSize() == OPERAND_DATA_SIZE_UNKNOWN)
    {
        data_size = get_data_size_for_reg(right->getFirstRegisterBranch()->getValue());
        right->setDataSize(data_size);
        if (instruction_branch->hasLeftBranch()
                && left->getDataSize() == OPERAND_DATA_SIZE_UNKNOWN && !left->isAccessingMemory())
        {
            // Ok lets set the left to the rights data size
            left->setDataSize(data_size);
        }
    }

    // We should only calculate the data size of the left branch if the left branches data size is unknown
    if (instruction_branch->hasLeftBranch()
            && left->getDataSize() == OPERAND_DATA_SIZE_UNKNOWN)
    {
        calculate_data_size_for_operand(left);
    }


    // We should only calculate the data size for the right operand if the size is unknown
    if (instruction_branch->hasRightBranch()
            && right->getDataSize() == OPERAND_DATA_SIZE_UNKNOWN)
    {
        calculate_data_size_for_operand(right);
    }
}

void Assembler8086::add_must_fits_if_required(std::shared_ptr<InstructionBranch> ins_branch)
{
    // We may need to register a offset must fit for later on incase label offsets are too far for the instruction
    if (ins_branch->hasOnlyLeftOperandBranch())
    {
        std::shared_ptr<OperandBranch> left = ins_branch->getLeftBranch();
        if (left->isOnlyImmediate() &&
                left->hasIdentifierBranch())
        {
            std::string iden_name = left->getIdentifierBranch()->getValue();
            if (get_identifier_type(iden_name) == IDENTIFIER_TYPE_LABEL)
            {
                // Ok lets get the label branch 
                std::shared_ptr<LabelBranch> label_branch = get_label_branch(left->getIdentifierBranch()->getValue());
                if (label_branch != NULL)
                {
                    MUST_FIT_TYPE must_fit = (left->getDataSize() == OPERAND_DATA_SIZE_BYTE ? MUST_FIT_8_BIT_SIGNED : MUST_FIT_16_BIT_SIGNED);
                    label_branch->getMustFitTable()->addMustFit(must_fit, ins_branch, left);
                }
            }

        }
    }

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
    if (_register == "bx" && second_reg == "si" || _register == "si" && second_reg == "bx")
    {
        return 0;
    }
    else if (_register == "bx" && second_reg == "di" || _register == "di" && second_reg == "bx")
    {
        return 1;
    }
    else if (_register == "bp" && second_reg == "si" || _register == "si" && second_reg == "bp")
    {
        return 2;
    }
    else if (_register == "bp" && second_reg == "di" || _register == "di" && second_reg == "bp")
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
            _register == "es" ||
            _register == "di" ||
            _register == "si");
}

void Assembler8086::ins_info_except()
{
    throw AssemblerException("Improperly formatted ins_info array for instruction type: " + std::to_string(cur_ins_type));
}