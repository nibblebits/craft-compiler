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
    
    void inline make_label(std::string label);
    void inline make_exact_label(std::string label);
    std::string build_unique_label();
    void make_variable(std::string name, std::string datatype, std::shared_ptr<Branch> value_exp);
    void make_mem_assignment(std::string dest, std::shared_ptr<Branch> value_exp);
    void make_expression(std::shared_ptr<Branch> exp);
    void make_expression_left(std::shared_ptr<Branch> exp, std::string register_to_store);
    void make_math_instruction(std::string op, std::string first_reg, std::string second_reg = "");
    void make_move_reg_variable(std::string reg_name, std::string var_name);
    void make_move_variable_address(std::string reg_name, std::string var_name);
    
    void handle_global_var_def(std::shared_ptr<VDEFBranch> vdef_branch);
    void handle_function(std::shared_ptr<FuncBranch> func_branch);
    void handle_func_args(std::shared_ptr<Branch> arguments);
    void handle_func_body(std::shared_ptr<Branch> body);
    void handle_stmt(std::shared_ptr<Branch> branch);
    void handle_function_call(std::shared_ptr<FuncCallBranch> branch);
    void handle_scope_assignment(std::shared_ptr<AssignBranch> assign_branch);
    void handle_scope_return(std::shared_ptr<Branch> branch);
    void handle_move_pointed_to_reg(std::string reg, std::shared_ptr<Branch> branch);
    void handle_compare_expression();
    
    int getFunctionArgumentIndex(std::string arg_name);
    int getBPOffsetForArgument(std::string arg_name);
    int getScopeVariableIndex(std::string arg_name);
    int getBPOffsetForScopeVariable(std::string arg_name);
    int getVariableType(std::string arg_name);
    std::string getASMAddressForVariable(std::string var_name);
    
    std::shared_ptr<Branch> getScopeVariable(std::string var_name);
    std::shared_ptr<Branch> getFunctionArgumentVariable(std::string arg_name);
    
    bool isVariablePointer(std::string var_name);
    
    void generate_global_branch(std::shared_ptr<Branch> branch);
    void assemble(std::string assembly);
    std::shared_ptr<Linker> getLinker();
private:
    std::shared_ptr<Linker> linker;
    std::vector<std::shared_ptr<Branch>> func_arguments;
    std::vector<std::shared_ptr<Branch>> scope_variables;
    std::string cmp_exp_false_label_name;
    std::string cmp_exp_end_label_name;
    bool is_cmp_expression;
    int current_label_index;
};

#endif /* CODEGEN8086_H */

