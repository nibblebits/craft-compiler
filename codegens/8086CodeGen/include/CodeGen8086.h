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

class CodeGen8086 : public CodeGenerator
{
public:
    CodeGen8086(Compiler* compiler);
    virtual ~CodeGen8086();

    inline void make_label(std::string label);
    inline void make_exact_label(std::string label);
    inline std::string make_unique_label();
    inline std::string build_unique_label();
    void make_variable(std::string name, std::string datatype, std::shared_ptr<Branch> value_exp);
    void make_mem_assignment(std::string dest, std::shared_ptr<Branch> value_exp, bool is_word = false);
    void make_expression(std::shared_ptr<Branch> exp);
    void make_expression_part(std::shared_ptr<Branch> exp, std::string register_to_store);
    void make_expression_left(std::shared_ptr<Branch> exp, std::string register_to_store);
    void make_expression_right(std::shared_ptr<Branch> exp);
    void make_math_instruction(std::string op, std::string first_reg, std::string second_reg = "");
    void make_move_reg_variable(std::string reg_name, std::shared_ptr<Branch> var_branch);
    void make_move_var_addr_to_reg(std::string reg_name, std::shared_ptr<Branch> var_branch);
    void make_var_assignment(std::shared_ptr<Branch> var_branch, std::shared_ptr<Branch> value, bool pointer_assignment);

    void handle_global_var_def(std::shared_ptr<VDEFBranch> vdef_branch);
    void handle_structure(std::shared_ptr<STRUCTBranch> struct_branch);
    void handle_function(std::shared_ptr<FuncBranch> func_branch);
    void handle_func_args(std::shared_ptr<Branch> arguments);
    void handle_body(std::shared_ptr<Branch> body);
    void handle_stmt(std::shared_ptr<Branch> branch);
    void handle_function_call(std::shared_ptr<FuncCallBranch> branch);
    void handle_scope_assignment(std::shared_ptr<AssignBranch> assign_branch);
    void handle_scope_return(std::shared_ptr<Branch> branch);
    void handle_move_pointed_to_reg(std::string reg, std::shared_ptr<Branch> branch);
    void handle_compare_expression();
    void handle_scope_variable_declaration(std::shared_ptr<Branch> branch);
    void handle_if_stmt(std::shared_ptr<IFBranch> branch);
    void handle_for_stmt(std::shared_ptr<FORBranch> branch);

    int getSizeOfVariableBranch(std::shared_ptr<VDEFBranch> vdef_branch);
    int getFunctionArgumentIndex(std::shared_ptr<Branch> var_branch);
    int getBPOffsetForArgument(std::shared_ptr<Branch> var_branch);
    std::shared_ptr<STRUCTBranch> getStructure(std::string struct_name);
    std::shared_ptr<STRUCTBranch> getStructureFromScopeVariable(std::shared_ptr<Branch> branch);
    std::shared_ptr<Branch> getVariableFromStructure(std::shared_ptr<STRUCTBranch> structure, std::string var_name);
    int getStructureVariableOffset(std::string struct_name, std::string var_name);
    int getStructureVariableOffset(std::shared_ptr<STRUCTBranch> struct_branch, std::string var_name);
    int getPosForStructureVariable(std::shared_ptr<Branch> branch);
    int getStructSize(std::string struct_name);
    int getBPOffsetForScopeVariable(std::shared_ptr<Branch> var_branch);
    bool hasScopeVariable(std::shared_ptr<Branch> var_branch);
    bool hasArgumentVariable(std::shared_ptr<Branch> var_branch);
    int getVariableType(std::shared_ptr<Branch> var_branch);
    int getSumOfScopeVariablesSizeSoFar();
    std::string getASMAddressForVariable(std::shared_ptr<Branch> var_branch);

    std::shared_ptr<VDEFBranch> getVariable(std::shared_ptr<Branch> var_branch);
    std::shared_ptr<Branch> getScopeVariable(std::shared_ptr<Branch> var_branch);
    std::shared_ptr<Branch> getFunctionArgumentVariable(std::shared_ptr<Branch> var_branch);

    bool isVariablePointer(std::shared_ptr<Branch> var_branch);
    inline bool is_cmp_logic_operator_nothing_or_and();

    void generate_global_branch(std::shared_ptr<Branch> branch);
    void assemble(std::string assembly);
    std::shared_ptr<Linker> getLinker();
private:
    Compiler* compiler;
    std::shared_ptr<Linker> linker;
    std::vector<std::shared_ptr<Branch>> func_arguments;
    std::vector<std::shared_ptr<Branch>> scope_variables;
    std::vector<std::shared_ptr<STRUCTBranch>> structures;
    std::shared_ptr<STRUCTBranch> last_structure;

    std::string cmp_exp_true_label_name;
    std::string cmp_exp_false_label_name;
    std::string cmp_exp_end_label_name;
    std::string cmp_exp_last_logic_operator;
    bool is_cmp_expression;
    int current_label_index;

};

#endif /* CODEGEN8086_H */

