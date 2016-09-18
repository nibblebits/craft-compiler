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
#include "branches.h"

enum
{
    GLOBAL_VARIABLE,
    ARGUMENT_VARIABLE,
    SCOPE_VARIABLE
};

class CodeGen8086 : public CodeGenerator {
public:
    CodeGen8086(Compiler* compiler);
    virtual ~CodeGen8086();
    
    void make_label(std::string label);
    void make_variable(std::string name, std::string datatype, std::shared_ptr<Branch> value_exp);
    void make_assignment(std::string pos, std::shared_ptr<Branch> value_exp);
    void make_expression(std::shared_ptr<Branch> exp);
    void make_math_instruction(std::string op, std::string first_reg, std::string second_reg = "");
    void make_move_reg_variable(std::string reg_name, std::string var_name);
    
    void handle_global_var_def(std::shared_ptr<VDEFBranch> vdef_branch);
    void handle_function(std::shared_ptr<FuncBranch> func_branch);
    void handle_func_args(std::shared_ptr<Branch> arguments);
    void handle_func_body(std::shared_ptr<Branch> body);
    void handle_stmt(std::shared_ptr<Branch> branch);
    void handle_scope_assignment(std::shared_ptr<AssignBranch> assign_branch);
    void handle_scope_return(std::shared_ptr<Branch> branch);
    
    int getFunctionArgumentIndex(std::string arg_name);
    int getBPOffsetForArgument(std::string arg_name);
    int getScopeVariableIndex(std::string arg_name);
    int getBPOffsetForScopeVariable(std::string arg_name);
    int getVariableType(std::string arg_name);
    
    void generate_global_branch(std::shared_ptr<Branch> branch);
    void assemble(std::string assembly);
    std::shared_ptr<Linker> getLinker();
private:
    std::shared_ptr<Linker> linker;
    std::vector<std::shared_ptr<Branch>> func_arguments;
    std::vector<std::shared_ptr<Branch>> scope_variables;
};

#endif /* CODEGEN8086_H */
