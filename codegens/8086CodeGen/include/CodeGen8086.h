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
 * File:   CodeGen8086.h
 * Author: Daniel McCarthy
 *
 * Created on 14 September 2016, 03:40
 */

#ifndef CODEGEN8086_H
#define CODEGEN8086_H

#define EXTERNAL_USE

#include "GoblinByteCodeLinker.h"
#include "CodeGenerator.h"
#include "VDEFBranch.h"

class CodeGen8086 : public CodeGenerator {
public:
    CodeGen8086(Compiler* compiler);
    virtual ~CodeGen8086();
    
    void make_label(std::string label);
    void make_variable(std::string name, std::string datatype, std::shared_ptr<Branch> value_exp);
    void make_assignment(std::string var_name, std::shared_ptr<Branch> value_exp);
    void make_expression(std::shared_ptr<Branch> exp);
    void make_math_instruction(std::string op, std::string first_reg, std::string second_reg = "");
    
    void handle_global_var_def(std::shared_ptr<VDEFBranch> vdef_branch);
    void generate_global_branch(std::shared_ptr<Branch> branch);
    void assemble(std::string assembly);
    std::shared_ptr<Linker> getLinker();
private:
    std::shared_ptr<Linker> linker;
};

#endif /* CODEGEN8086_H */

