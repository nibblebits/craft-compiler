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

#include "CodeGenerator.h"

// Instruction set
enum
{
    NOP,
    JMP,
    CALL,
    SPUSH,
    SPOP,
    APUSH,
    APOP,
    MEMSET_SP_RELATIVE,
    SUBSP,
    ADDSP,
    SADD,
    SSUB,
    SMUL,
    SDIV
};

class GoblinByteCodeGenerator : public CodeGenerator
{
public:
    GoblinByteCodeGenerator(Compiler* compiler);
    virtual ~GoblinByteCodeGenerator();
    
    void handleScope(std::shared_ptr<Branch> branch);
    void handleExpression(std::shared_ptr<Branch> branch);
    // Unsure what to name this at the moment
    void handleEntity(std::shared_ptr<Branch> branch);
    virtual void generate(std::shared_ptr<Tree> tree);
    virtual void generateFromBranch(std::shared_ptr<Branch> branch);
private:
    int saved_pos;
};

#endif /* GOBLINBYTECODEGENERATOR_H */

