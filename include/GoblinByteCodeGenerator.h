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
 * File:   GoblinByteCodeGenerator.h
 * Author: Daniel McCarthy
 *
 * Created on 20 June 2016, 01:07
 */

#ifndef GOBLINBYTECODEGENERATOR_H
#define GOBLINBYTECODEGENERATOR_H

#include "GoblinByteCodeLinker.h"
#include "CodeGenerator.h"

// Instruction set

enum
{
    NOP = 0x0,
    JMP = 0x50,
    CALL = 0x51,
    SPUSH = 0x52,
    SPOP = 0x53,
    APUSH = 0x54,
    APOP = 0x55,
    SPOP_TO_DP_RELATIVE = 0x56,
    SUBDP = 0x57,
    ADDDP = 0x58,
    SADD = 0x59,
    SSUB = 0x60,
    SMUL = 0x61,
    SDIV = 0x62,
    SPUSH_MEMORY_VALUE_DP_RELATIVE = 0x63
};

class GoblinByteCodeGenerator : public CodeGenerator
{
public:
    GoblinByteCodeGenerator(Compiler* compiler, std::string code_gen_desc);
    virtual ~GoblinByteCodeGenerator();

    // Unsure what to name this at the moment
    bool handleEntity(std::shared_ptr<Branch> branch);
    void createStackMathInstruction(std::string op);
    virtual void generate(std::shared_ptr<Tree> tree);
    virtual void generateFromBranch(std::shared_ptr<Branch> branch);

    virtual std::shared_ptr<Linker> getLinker();
    virtual void scope_start(std::shared_ptr<Branch> branch);
    virtual void scope_assign_start(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> var);
    virtual void scope_assign_end(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> var);
    virtual void scope_func_call(std::shared_ptr<Branch> branch, std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments);
    virtual void scope_end(std::shared_ptr<Branch> branch);
    virtual void scope_handle_exp(std::shared_ptr<Branch> branch);
    virtual void scope_handle_number(std::shared_ptr<Branch> branch);

private:
    int saved_pos;
    std::shared_ptr<Linker> linker;
};

#endif /* GOBLINBYTECODEGENERATOR_H */

