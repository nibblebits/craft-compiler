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

#include <deque>
#include "GoblinByteCodeLinker.h"
#include "CodeGenerator.h"
#include "branches.h"

#define POINTER_SIZE 2

enum
{
    GLOBAL_VARIABLE,
    ARGUMENT_VARIABLE,
    SCOPE_VARIABLE
};

struct HANDLING_POINTER
{
    bool is_handling;
    std::shared_ptr<VDEFBranch> pointer_selected_variable;
};

struct VARIABLE_ADDRESS
{
    int var_type;
    std::string segment;
    std::string op;
    int offset;

    inline std::string to_string()
    {
        // No OP? then just return the segment.
        if (op == "")
        {
            return segment;
        }

        return segment + op + std::to_string(offset);
    }
};

class CodeGen8086 : public CodeGenerator
{
public:
    CodeGen8086(Compiler* compiler);
    virtual ~CodeGen8086();

    virtual struct formatted_segment format_segment(std::string segment_name);

    inline void make_label(std::string label, std::string segment = "code");
    inline void make_exact_label(std::string label, std::string segment = "code");
    inline std::string make_unique_label(std::string segment = "code");
    inline std::string build_unique_label();

    std::string make_string(std::shared_ptr<Branch> string_branch);
    void make_variable(std::string name, std::string datatype, std::shared_ptr<Branch> value_exp);
    void make_mem_assignment(std::string dest, std::shared_ptr<Branch> value_exp, bool is_word = false, std::function<void() > assignment_val_processed = NULL);
    void make_expression(std::shared_ptr<Branch> exp, std::function<void() > exp_start_func = NULL, std::function<void() > exp_end_func = NULL, bool postpone_pointer=true);
    void make_expression_part(std::shared_ptr<Branch> exp, std::string register_to_store);
    void make_expression_left(std::shared_ptr<Branch> exp, std::string register_to_store);
    void make_expression_right(std::shared_ptr<Branch> exp);
    void make_math_instruction(std::string op, std::string first_reg, std::string second_reg = "");
    void make_move_reg_variable(std::string reg_name, std::shared_ptr<VarIdentifierBranch> var_branch);
    void make_move_var_addr_to_reg(std::string reg_name, std::shared_ptr<VarIdentifierBranch> var_branch);
    void make_array_offset_instructions(std::shared_ptr<ArrayIndexBranch> array_branch, int size_p_elem = 1);
    void make_move_mem_to_mem(VARIABLE_ADDRESS &dest_loc, VARIABLE_ADDRESS &from_loc, int size);
    void make_move_mem_to_mem(std::string dest_loc, std::string from_loc, int size);
    void make_var_access_rel_base(std::shared_ptr<VarIdentifierBranch> var_branch, std::shared_ptr<VDEFBranch>* vdef_in_question_branch = NULL, std::shared_ptr<VarIdentifierBranch>* var_access_iden_branch = NULL, std::string base_reg = "bx", std::shared_ptr<STRUCTBranch> current_struct = NULL);
    std::string make_var_access(std::shared_ptr<VarIdentifierBranch> var_branch, bool pointer_access = false, std::shared_ptr<VDEFBranch>* vdef_in_question_branch = NULL, std::shared_ptr<VarIdentifierBranch>* var_access_iden_branch = NULL, std::string base_reg = "bx");
    void make_var_assignment(std::shared_ptr<Branch> var_branch, std::shared_ptr<Branch> value);

    void calculate_scope_size(std::shared_ptr<Branch> body_branch);
    void reset_scope_size();

    void handle_ptr(std::shared_ptr<PTRBranch> ptr_branch);
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
    void handle_scope_variable_declaration(std::shared_ptr<VDEFBranch> branch);
    void handle_if_stmt(std::shared_ptr<IFBranch> branch);
    void handle_for_stmt(std::shared_ptr<FORBranch> branch);

    inline bool has_postponed_pointer_handling();
    void postpone_pointer_handling();
    void prepone_pointer_handling();
    
    int getSizeOfVariableBranch(std::shared_ptr<VDEFBranch> vdef_branch);
    int getFunctionArgumentIndex(std::shared_ptr<Branch> var_branch);
    int getBPOffsetForArgument(std::shared_ptr<Branch> var_branch);
    int getOffsetForGlobalVariable(std::shared_ptr<Branch> var_branch);
    std::shared_ptr<STRUCTBranch> getStructure(std::string struct_name);
    std::shared_ptr<STRUCTBranch> getStructureFromScopeVariable(std::shared_ptr<Branch> branch);
    std::shared_ptr<VDEFBranch> getVariableFromStructure(std::shared_ptr<STRUCTBranch> structure, std::string var_name);
    int getStructureVariableOffset(std::string struct_name, std::string var_name);
    int getStructureVariableOffset(std::shared_ptr<STRUCTBranch> struct_branch, std::string var_name);
    int getPosForStructureVariable(std::shared_ptr<Branch> branch);
    int getStructSize(std::string struct_name);
    int getBPOffsetForScopeVariable(std::shared_ptr<Branch> var_branch);
    bool hasGlobalVariable(std::shared_ptr<VarIdentifierBranch> var_branch);
    bool hasScopeVariable(std::shared_ptr<Branch> var_branch);
    bool hasArgumentVariable(std::shared_ptr<Branch> var_branch);
    int getVariableType(std::shared_ptr<Branch> var_branch);
    std::string convert_full_reg_to_low_reg(std::string reg);

    struct VARIABLE_ADDRESS getASMAddressForVariable(std::shared_ptr<Branch> var_branch);
    std::string getASMAddressForVariableFormatted(std::shared_ptr<Branch> var_branch);

    std::shared_ptr<VDEFBranch> getVariable(std::shared_ptr<Branch> var_branch);
    std::shared_ptr<Branch> getScopeVariable(std::shared_ptr<Branch> var_branch);
    std::shared_ptr<Branch> getGlobalVariable(std::shared_ptr<Branch> var_branch);
    std::shared_ptr<Branch> getFunctionArgumentVariable(std::shared_ptr<Branch> var_branch);

    bool isVariablePointer(std::shared_ptr<Branch> var_branch);
    inline bool is_cmp_logic_operator_nothing_or_and();
    inline bool is_alone_var_to_be_word(std::shared_ptr<VDEFBranch> vdef_branch, bool ignore_pointer = false);
    inline bool is_alone_var_to_be_word(std::shared_ptr<VarIdentifierBranch> var_branch, bool ignore_pointer = false);

    void generate_global_branch(std::shared_ptr<Branch> branch);
    void assemble(std::string assembly);
    std::shared_ptr<Linker> getLinker();
private:
    Compiler* compiler;
    std::shared_ptr<Linker> linker;
    std::vector<std::shared_ptr<Branch>> func_arguments;
    std::vector<std::shared_ptr<VDEFBranch>> global_variables;
    std::vector<std::shared_ptr<VDEFBranch>> scope_variables;
    std::vector<std::shared_ptr<STRUCTBranch>> structures;
    std::shared_ptr<STRUCTBranch> last_structure;

    std::string cmp_exp_true_label_name;
    std::string cmp_exp_false_label_name;
    std::string cmp_exp_end_label_name;
    std::string cmp_exp_last_logic_operator;

    bool is_cmp_expression;
    bool do_signed;
    bool is_handling_pointer;

    std::shared_ptr<VDEFBranch> pointer_selected_variable;
    std::shared_ptr<VDEFBranch> last_found_var_access_variable;
    
    /*  Holds pointer handling information for a given pointer 
     stack is required due to sub expressions that are non pointer related. */
    std::deque<HANDLING_POINTER> current_pointers_to_handle;
    int current_label_index;
    int scope_size;

};

#endif /* CODEGEN8086_H */

