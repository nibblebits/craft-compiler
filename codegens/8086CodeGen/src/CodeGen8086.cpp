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
 * File:   CodeGen8086.cpp
 * Author: Daniel McCarthy
 *
 * Created on 14 September 2016, 03:40
 * 
 * Description: 
 */

#include "CodeGen8086.h"

CodeGen8086::CodeGen8086(Compiler* compiler) : CodeGenerator(compiler, "8086 CodeGenerator")
{
    this->linker = std::shared_ptr<Linker>(new GoblinByteCodeLinker(compiler));
    this->compiler = compiler;
    this->current_label_index = 0;
    this->is_cmp_expression = false;
    this->do_signed = false;
    this->handling_pointer = false;
    this->pointer_var_branch = NULL;
}

CodeGen8086::~CodeGen8086()
{
}

void CodeGen8086::make_label(std::string label)
{
    do_asm("_" + label + ":");
}

void CodeGen8086::make_exact_label(std::string label)
{
    do_asm(label + ":");
}

std::string CodeGen8086::build_unique_label()
{
    std::string label_name = std::to_string(this->current_label_index);
    this->current_label_index++;
    return "_" + label_name;
}

std::string CodeGen8086::make_unique_label()
{
    std::string label_name = build_unique_label();
    make_exact_label(label_name);
    return label_name;
}

void CodeGen8086::make_variable(std::string name, std::string datatype, std::shared_ptr<Branch> value_exp)
{
    make_label(name);
    std::string result;
    if (datatype == "uint8")
    {
        result = "db";
    }
    else if (datatype == "uint16")
    {
        result = "dw";
    }

    result += ": 0";
    do_asm(result);

    // Do we need to process an expression?
    if (value_exp != NULL)
    {
        // Ok in that case this declaration is also an assignment
        make_mem_assignment(name, value_exp);
    }
}

void CodeGen8086::make_mem_assignment(std::string dest, std::shared_ptr<Branch> value_exp, bool is_word)
{
    // We have a value expression here so make it.
    make_expression(value_exp);

    // Handle any compare expression if any
    if (this->is_cmp_expression)
    {
        handle_compare_expression();
    }

    // Now we must assign the variable with the expression result`
    if (is_word)
    {
        do_asm("mov [" + dest + "], ax");
    }
    else
    {
        do_asm("mov [" + dest + "], al");
    }
}

void CodeGen8086::make_expression(std::shared_ptr<Branch> exp)
{
    if (exp->getType() != "E")
    {
        make_expression_left(exp, "ax");
    }
    else
    {
        std::string exp_val = exp->getValue();
        if (compiler->isLogicalOperator(exp_val))
        {
            this->cmp_exp_last_logic_operator = exp_val;
        }
        else if (compiler->isCompareOperator(exp_val))
        {
            // Setup compare labels
            if (!this->is_cmp_expression)
            {
                this->cmp_exp_false_label_name = build_unique_label();
                this->cmp_exp_end_label_name = build_unique_label();
                this->cmp_exp_true_label_name = build_unique_label();
                is_cmp_expression = true;
            }
        }

        std::shared_ptr<Branch> left = exp->getFirstChild();
        std::shared_ptr<Branch> right = exp->getSecondChild();

        if (left->getType() == "E")
        {
            make_expression(left);
        }
        else if (right->getType() != "E")
        {
            make_expression_left(left, "ax");
        }

        // Save the AX if we need to but not if we have a logical operator
        if (
                !compiler->isLogicalOperator(exp->getValue()) &&
                left->getType() == "E" &&
                right->getType() == "E")
        {
            do_asm("push ax");
        }

        /* Check to see if the left and right branches are both logical operators 
        if this is the case we must handle the compare expression to make expressions such as
        (5 > 4 && 5 < 3) || (5 == 5 && 4 == 3) possible.*/
        if (exp->getValue() == "||" &&
                left->getType() == "E" &&
                right->getType() == "E" &&
                compiler->isLogicalOperator(left->getValue()) &&
                compiler->isLogicalOperator(right->getValue()))
        {
            handle_compare_expression();
            this->cmp_exp_false_label_name = build_unique_label();
            this->cmp_exp_end_label_name = build_unique_label();
            this->cmp_exp_true_label_name = build_unique_label();
            is_cmp_expression = true;
        }


        if (right->getType() == "E")
        {
            make_expression(right);
            if (left->getType() != "E")
            {
                make_expression_left(left, "cx");
            }
        }
        else
        {
            make_expression_right(right);
        }

        // Restore the AX register if we need to but not if a logical operator is present.
        if (
                !compiler->isLogicalOperator(exp->getValue()) &&
                left->getType() == "E" &&
                right->getType() == "E")
        {

            do_asm("pop cx");
        }
        // Don't make math instructions for logical operators.
        if (!compiler->isLogicalOperator(exp->getValue()))
        {
            if (this->do_signed)
            {
                make_math_instruction(exp->getValue(), "al", "cl");
            }
            else
            {
                make_math_instruction(exp->getValue(), "ax", "cx");
            }
        }
    }
}

void CodeGen8086::make_expression_part(std::shared_ptr<Branch> exp, std::string register_to_store)
{
    if (exp->getType() == "number")
    {
        do_asm("mov " + register_to_store + ", " + exp->getValue());
    }
    else if (exp->getType() == "VAR_IDENTIFIER")
    {
        /* If we are currently inside a pointer *(inside here) then we need to set this variable to the
         pointer variable branch so once we are done we have a reference of what data type we are referencing to
         */
        if (this->pointer_var_branch == NULL &&
                this->handling_pointer)
        {
            std::shared_ptr<VDEFBranch> var_branch = getVariable(exp);
            if (var_branch->isPointer())
            {
                this->pointer_var_branch = var_branch;
            }
        }

        // This is a variable so set register to store to the value of this variable
        make_move_reg_variable(register_to_store, exp);
    }
    else if (exp->getType() == "PTR")
    {
        std::shared_ptr<PTRBranch> ptr_branch = std::dynamic_pointer_cast<PTRBranch>(exp);
        handle_ptr(ptr_branch);
    }
    else if (exp->getType() == "FUNC_CALL")
    {
        // This is a function call so handle it
        std::shared_ptr<FuncCallBranch> func_call_branch = std::dynamic_pointer_cast<FuncCallBranch>(exp);
        handle_function_call(func_call_branch);
    }
    else if (exp->getType() == "ADDRESS_OF")
    {
        // Move the address of the variable to the AX register
        std::shared_ptr<AddressOfBranch> address_of_branch = std::dynamic_pointer_cast<AddressOfBranch>(exp);
        std::shared_ptr<VarIdentifierBranch> var_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(address_of_branch->getVariableBranch());
        make_move_var_addr_to_reg(register_to_store, var_branch);
    }
    else if (exp->getType() == "STRUCT_ACCESS")
    {
        make_move_reg_variable(register_to_store, exp);
    }
}

void CodeGen8086::make_expression_left(std::shared_ptr<Branch> exp, std::string register_to_store)
{
    make_expression_part(exp, register_to_store);
}

void CodeGen8086::make_expression_right(std::shared_ptr<Branch> exp)
{
    if (exp->getType() == "FUNC_CALL")
    {
        /*
         * This is a function call, we must push AX as at this point AX is set to something,
         * the AX register is the register used to return data to the function caller.
         * Therefore the previous AX register must be saved
         */

        std::shared_ptr<FuncCallBranch> func_call_branch = std::dynamic_pointer_cast<FuncCallBranch>(exp);

        // PROBABLY A SERIOUS PROBLEM HERE CHECK IT OUT...

        // Save AX
        do_asm("push ax");
        handle_function_call(func_call_branch);
        // Since AX now contains returned value we must move it to register CX as this is where right operands get stored of any expression
        do_asm("mov cx, ax");
        // Restore AX
        do_asm("pop ax");
    }
    else
    {
        make_expression_part(exp, "cx");
    }

}

void CodeGen8086::make_math_instruction(std::string op, std::string first_reg, std::string second_reg)
{
    if (op == "+")
    {
        do_asm("add " + first_reg + ", " + second_reg);
    }
    else if (op == "-")
    {
        do_asm("sub " + first_reg + ", " + second_reg);
    }
    else if (op == "*")
    {
        /*
         * If the first register is AX then set the first register to the second register as "mul" instruction
         * uses AX as first reg regardless */
        if (first_reg == "ax")
        {
            first_reg = second_reg;
        }
        do_asm("mul " + first_reg);
    }
    else if (op == "/")
    {
        /*
         * If the first register is ax then set the first register to the second register as "div" instruction
         * uses ax as first reg regardless */
        if (first_reg == "ax")
        {
            first_reg = second_reg;
        }
        do_asm("div " + first_reg);

    }
    else if (
            op == "!=" ||
            op == "==" ||
            op == "<=" ||
            op == ">=" ||
            op == ">" ||
            op == "<")
    {

        // We must compare
        do_asm("cmp " + first_reg + ", " + second_reg);

        if (op == "==")
        {
            if (is_cmp_logic_operator_nothing_or_and())
            {
                do_asm("jne " + this->cmp_exp_false_label_name);
            }
            else
            {
                // This is a logical else "||"
                do_asm("je " + this->cmp_exp_true_label_name);
            }
        }
        else if (op == "!=")
        {
            if (is_cmp_logic_operator_nothing_or_and())
            {
                do_asm("je " + this->cmp_exp_false_label_name);
            }
            else
            {
                do_asm("jne " + this->cmp_exp_true_label_name);
            }
        }
        else if (op == "<=")
        {
            if (is_cmp_logic_operator_nothing_or_and())
            {
                if (this->do_signed)
                {
                    do_asm("jg " + this->cmp_exp_false_label_name);
                }
                else
                {
                    do_asm("ja " + this->cmp_exp_false_label_name);
                }

            }
            else
            {
                if (this->do_signed)
                {
                    do_asm("jle " + this->cmp_exp_true_label_name);
                }
                else
                {
                    do_asm("jbe " + this->cmp_exp_true_label_name);
                }
            }
        }
        else if (op == ">=")
        {
            if (is_cmp_logic_operator_nothing_or_and())
            {
                if (this->do_signed)
                {
                    do_asm("jl " + this->cmp_exp_false_label_name);
                }
                else
                {
                    do_asm("jb " + this->cmp_exp_false_label_name);
                }
            }
            else
            {
                if (this->do_signed)
                {
                    do_asm("jge " + this->cmp_exp_true_label_name);
                }
                else
                {
                    do_asm("jae " + this->cmp_exp_true_label_name);
                }
            }
        }
        else if (op == "<")
        {
            if (is_cmp_logic_operator_nothing_or_and())
            {
                if (this->do_signed)
                {
                    do_asm("jge " + this->cmp_exp_false_label_name);
                }
                else
                {
                    do_asm("jae " + this->cmp_exp_false_label_name);
                }
            }
            else
            {
                if (this->do_signed)
                {
                    do_asm("jl " + this->cmp_exp_true_label_name);
                }
                else
                {
                    do_asm("jb " + this->cmp_exp_true_label_name);
                }
            }
        }
        else if (op == ">")
        {
            if (is_cmp_logic_operator_nothing_or_and())
            {
                if (this->do_signed)
                {
                    do_asm("jle " + this->cmp_exp_false_label_name);
                }
                else
                {
                    do_asm("jbe " + this->cmp_exp_false_label_name);
                }
            }
            else
            {
                if (this->do_signed)
                {
                    do_asm("jg " + this->cmp_exp_true_label_name);
                }
                else
                {
                    do_asm("ja " + this->cmp_exp_true_label_name);
                }
            }
        }

        this->do_signed = false;
    }
    else
    {
        throw CodeGeneratorException("void CodeGen8086::make_math_instruction(std::string op): expecting a valid operator");
    }
}

void CodeGen8086::make_move_reg_variable(std::string reg, std::shared_ptr<Branch> var_branch)
{
    std::shared_ptr<VDEFBranch> variable_branch = getVariable(var_branch);
    if (variable_branch->isSigned())
    {
        this->do_signed = true;
    }

    std::string asm_addr = getASMAddressForVariableFormatted(var_branch);
    std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
    if (var_iden_branch->hasRootArrayIndexBranch())
    {
        make_array_variable_access(var_iden_branch);
        // bx = correct memory location for memory
        do_asm("mov " + reg + ", [bx]");
    }
    else
    {
        do_asm("mov " + reg + ", [" + asm_addr + "]");
    }
}

void CodeGen8086::make_move_var_addr_to_reg(std::string reg_name, std::shared_ptr<VarIdentifierBranch> var_branch)
{
    int bp_offset;
    int var_type = getVariableType(var_branch);

    if (var_branch->hasRootArrayIndexBranch())
    {
        make_array_variable_access(var_branch);

        // Resulting address is stored in BX by default so we should only move it if needed
        if (reg_name != "bx")
        {
            do_asm("mov " + reg_name + ", bx");
        }

    }
    else
    {
        if (var_type == ARGUMENT_VARIABLE || var_type == SCOPE_VARIABLE)
        {
            do_asm("mov " + reg_name + ", bp");
        }
        switch (var_type)
        {
        case ARGUMENT_VARIABLE:
            bp_offset = getBPOffsetForArgument(var_branch);
            do_asm("add " + reg_name + ", " + std::to_string(bp_offset));
            break;
        case SCOPE_VARIABLE:
            bp_offset = getBPOffsetForScopeVariable(var_branch);
            do_asm("sub " + reg_name + ", " + std::to_string(bp_offset));
            break;
        }
    }
}

void CodeGen8086::make_array_offset_instructions(std::shared_ptr<ArrayIndexBranch> array_branch)
{
    std::shared_ptr<ArrayIndexBranch> current_branch = array_branch;

    int c = 0;
    while (true)
    {
        std::shared_ptr<Branch> value_branch = current_branch->getValueBranch();
        // Make the expression of the array index
        make_expression(value_branch);
        do_asm("push ax");

        c++;
        if (current_branch->hasNextArrayIndexBranch())
        {
            current_branch = std::dynamic_pointer_cast<ArrayIndexBranch>(current_branch->getNextArrayIndexBranch());
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < c; i++)
    {
        if (i == 0)
        {
            do_asm("pop ax");
        }
        else
        {
            do_asm("pop bx");
            do_asm("mul bx");
        }
    }

}

void CodeGen8086::make_array_variable_access(std::shared_ptr<VarIdentifierBranch> var_branch)
{
    struct VARIABLE_ADDRESS var_addr = getASMAddressForVariable(var_branch);
    // We are handling an array
    std::shared_ptr<ArrayIndexBranch> root_array_index = std::dynamic_pointer_cast<ArrayIndexBranch>(var_branch->getRootArrayIndexBranch());
    make_array_offset_instructions(root_array_index);

    if (var_addr.var_type == GLOBAL_VARIABLE)
    {
        do_asm("mov bx, " + var_addr.offset);
        do_asm("add bx, ax");
    }
    else
    {
        do_asm("mov bx, " + var_addr.segment);
        if (var_addr.var_type == ARGUMENT_VARIABLE)
        {
            do_asm("add bx, " + std::to_string(var_addr.offset));
            do_asm("sub bx, ax");
        }
        else
        {
            do_asm("sub bx, " + std::to_string(var_addr.offset));
            do_asm("add bx, ax");
        }
    }
}

void CodeGen8086::make_var_assignment(std::shared_ptr<Branch> var_branch, std::shared_ptr<Branch> value)
{
    /* Note: PTR is pointer access it means we are trying to assign a value in memory pointed to by a pointer
     * it is not the same as the code further down this method that makes memory assignments to pointers as 2 bytes as pointers should
 hold 2 byte addresses in the 8086 processor.*/

    if (var_branch->getType() == "PTR")
    {
        std::shared_ptr<PTRBranch> ptr_branch = std::dynamic_pointer_cast<PTRBranch>(var_branch);
        handle_ptr(ptr_branch);
        // Ok we have handled the pointer now we need to set our value to it
        do_asm("mov bx, ax");
        make_mem_assignment("bx", value, false);
    }
    else
    {
        std::shared_ptr<VDEFBranch> variable_def = std::dynamic_pointer_cast<VDEFBranch>(getVariable(var_branch));
        bool is_word = (variable_def->isPointer() || variable_def->getDataTypeSize() == 2 ? true : false);
        std::string asm_addr = getASMAddressForVariableFormatted(var_branch);
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
        if (var_iden_branch->hasRootArrayIndexBranch())
        {
            make_array_variable_access(var_iden_branch);
            // bx = correct memory location for memory
            make_mem_assignment("bx", value, is_word);
        }
        else
        {
            make_mem_assignment(asm_addr, value, is_word);
        }
    }
}

void CodeGen8086::handle_ptr(std::shared_ptr<PTRBranch> ptr_branch)
{
    this->handling_pointer = true;
    std::shared_ptr<Branch> exp_branch = ptr_branch->getExpressionBranch();
    make_expression(exp_branch);
    this->handling_pointer = false;

    /* 
     * We have finished making the pointer expression, now we need to return the value that its
     * pointing to.
     */

    if (this->pointer_var_branch != NULL)
    {
        do_asm("mov bx, ax");
        do_asm("mov ax, [bx]");
        this->pointer_var_branch = NULL;
    }

}

void CodeGen8086::handle_global_var_def(std::shared_ptr<VDEFBranch> vdef_branch)
{
    throw new Exception("Global variables are not yet supported.");
    //std::string var_keyword_value = vdef_branch->getDataTypeBranch()->getValue();
    //std::string var_name_value = vdef_branch->getNameBranch()->getValue();
    //make_variable(var_name_value, var_keyword_value, vdef_branch->getValueExpBranch());
}

void CodeGen8086::handle_structure(std::shared_ptr<STRUCTBranch> struct_branch)
{
    this->structures.push_back(struct_branch);
}

void CodeGen8086::handle_function(std::shared_ptr<FuncBranch> func_branch)
{
    // Clear previous scope variables from other functions
    this->scope_variables.clear();

    std::shared_ptr<Branch> return_branch = func_branch->getReturnTypeBranch();
    std::shared_ptr<Branch> name_branch = func_branch->getNameBranch();
    std::shared_ptr<Branch> arguments_branch = func_branch->getArgumentsBranch();
    std::shared_ptr<Branch> body_branch = func_branch->getBodyBranch();

    // Make the function label
    make_label(name_branch->getValue());

    do_asm("push bp");
    do_asm("mov bp, sp");

    // Handle the arguments
    handle_func_args(arguments_branch);
    // Handle the body
    handle_body(body_branch);

}

void CodeGen8086::handle_func_args(std::shared_ptr<Branch> arguments)
{
    this->func_arguments.clear();
    for (std::shared_ptr<Branch> arg : arguments->getChildren())
    {
        this->func_arguments.push_back(arg);
    }
}

void CodeGen8086::handle_body(std::shared_ptr<Branch> body)
{
    for (std::shared_ptr<Branch> stmt : body->getChildren())
    {
        handle_stmt(stmt);
    }
}

void CodeGen8086::handle_stmt(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "ASSIGN")
    {
        std::shared_ptr<AssignBranch> assign_branch = std::dynamic_pointer_cast<AssignBranch>(branch);
        handle_scope_assignment(assign_branch);
    }
    else if (branch->getType() == "FUNC_CALL")
    {
        std::shared_ptr<FuncCallBranch> func_call_branch = std::dynamic_pointer_cast<FuncCallBranch>(branch);
        handle_function_call(func_call_branch);
    }
    else if (branch->getType() == "RETURN")
    {
        handle_scope_return(branch);
    }
    else if (branch->getType() == "V_DEF" ||
            branch->getType() == "STRUCT_DEF")
    {
        handle_scope_variable_declaration(branch);
    }
    else if (branch->getType() == "IF")
    {
        std::shared_ptr<IFBranch> if_branch = std::dynamic_pointer_cast<IFBranch>(branch);
        handle_if_stmt(if_branch);
    }
    else if (branch->getType() == "FOR")
    {
        std::shared_ptr<FORBranch> for_branch = std::dynamic_pointer_cast<FORBranch>(branch);
        handle_for_stmt(for_branch);
    }
}

void CodeGen8086::handle_function_call(std::shared_ptr<FuncCallBranch> branch)
{
    std::shared_ptr<Branch> func_name_branch = branch->getFuncNameBranch();
    std::shared_ptr<Branch> func_params_branch = branch->getFuncParamsBranch();

    std::vector<std::shared_ptr < Branch>> params = func_params_branch->getChildren();

    int t_scope_size = getSumOfScopeVariablesSizeSoFar();

    /* Subtract the stack pointer by the size of the scope at this time, this is required
     * as other wise the scope will be overwritten by the function call arguments */
    do_asm("sub sp, " + std::to_string(t_scope_size));

    // Parameters are treated as an expression, they must be pushed on backwards due to how the stack works
    for (int i = params.size() - 1; i >= 0; i--)
    {
        std::shared_ptr<Branch> param = params.at(i);
        make_expression(param);
        // Push the expression to the stack as this is a function call
        do_asm("push ax");
    }

    // Now call the function :)
    do_asm("call _" + func_name_branch->getValue());

    /* Restore the stack pointer to what it was to recycle the memory */
    do_asm("add sp, " + std::to_string((params.size() * 2) + t_scope_size));
}

void CodeGen8086::handle_scope_assignment(std::shared_ptr<AssignBranch> assign_branch)
{
    std::shared_ptr<Branch> var_to_assign_branch = assign_branch->getVariableToAssignBranch();
    std::shared_ptr<Branch> value = assign_branch->getValueBranch();

    make_var_assignment(var_to_assign_branch, value);

}

void CodeGen8086::handle_scope_return(std::shared_ptr<Branch> branch)
{
    if (branch->hasChildren())
    {
        // We have something to return
        std::shared_ptr<Branch> return_child = branch->getFirstChild();
        // AX will be set to the value to return once the expression is complete.
        make_expression(return_child);
    }


    // Pop from the stack back to the BP(Base Pointer) now we are leaving this function
    do_asm("pop bp");
    do_asm("ret");
}

void CodeGen8086::handle_move_pointed_to_reg(std::string reg, std::shared_ptr<Branch> branch)
{
    std::string asm_addr = getASMAddressForVariableFormatted(branch);
    // Save BX as its possible it could be taken due to a pointer to pointer assignment
    do_asm("push bx");
    do_asm("mov bx, [" + asm_addr + "]");
    do_asm("mov " + reg + ", [bx]");
    // Restore BX
    do_asm("pop bx");
}

void CodeGen8086::handle_compare_expression()
{
    // Check if this is a compare expression, this is used for expressions such as "a == 5"

    /* Do a jmp to the true label here only if the logic operator is nothing or "&&". 
     * This is required as the following expression: 10 == 10 || 13 == 13 && 12 == 12
      will cause the system to roll onto the false label should all be true*/

    if (is_cmp_logic_operator_nothing_or_and())
    {
        do_asm("jmp " + this->cmp_exp_true_label_name);
    }

    // Generate false label
    make_exact_label(this->cmp_exp_false_label_name);

    // Move zero to AX as this is false
    do_asm("mov ax, 0");
    // Jump to the end so it does not change to true
    do_asm("jmp " + this->cmp_exp_end_label_name);

    // Generate true label
    make_exact_label(this->cmp_exp_true_label_name);
    // Move one to the AX as this is true
    do_asm("mov ax, 1");
    // No need to generate any "jmp" instruction as naturally the code will run to the end

    // Generate end label
    make_exact_label(this->cmp_exp_end_label_name);

    // Now reset the compare expression
    this->is_cmp_expression = false;

}

void CodeGen8086::handle_scope_variable_declaration(std::shared_ptr<Branch> branch)
{
    // Register a scope variable
    this->scope_variables.push_back(branch);


    // Handle the variable declaration
    std::shared_ptr<VDEFBranch> def_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
    std::shared_ptr<Branch> variable_branch = def_branch->getVariableIdentifierBranch();
    std::shared_ptr<Branch> value_branch = def_branch->getValueExpBranch();

    // Are we assigning it to anything?
    if (value_branch != NULL)
    {
        make_var_assignment(variable_branch, value_branch);
    }
}

void CodeGen8086::handle_if_stmt(std::shared_ptr<IFBranch> branch)
{
    std::shared_ptr<Branch> exp_branch = branch->getExpressionBranch();
    std::shared_ptr<Branch> body_branch = branch->getBodyBranch();

    // Process the expression of the "IF" statement
    make_expression(exp_branch);

    // Handle the compare expression
    handle_compare_expression();

    // AX now contains true or false 
    std::string true_label = build_unique_label();
    std::string false_label = build_unique_label();

    do_asm("cmp ax, 0");
    do_asm("je " + false_label);
    // This is where we will jump if its true
    make_exact_label(true_label);

    // Handle the "IF" statements body.
    handle_body(body_branch);

    // This is where we will jump if its false, the body will never be run.
    make_exact_label(false_label);

    /* We check for ELSE IF below the false label as this is where the next IF statement
     will need to be checked, due to the way the code flows.*/

    // Is there an else if ?
    if (branch->hasElseIfBranch())
    {
        std::shared_ptr<IFBranch> else_if_branch = std::dynamic_pointer_cast<IFBranch>(branch->getElseIfBranch());
        // Ok we have an else if so lets handle it
        handle_if_stmt(else_if_branch);
    }
    else if (branch->hasElseBranch())
    {
        /* ELSE statements also need to be below the false label due to the way the code flows.*/
        std::shared_ptr<ELSEBranch> else_branch = std::dynamic_pointer_cast<ELSEBranch>(branch->getElseBranch());
        std::shared_ptr<Branch> else_body_branch = else_branch->getBodyBranch();

        // Handle the else's body
        handle_body(else_body_branch);
    }
}

void CodeGen8086::handle_for_stmt(std::shared_ptr<FORBranch> branch)
{
    std::shared_ptr<Branch> init_branch = branch->getInitBranch();
    std::shared_ptr<Branch> cond_branch = branch->getCondBranch();
    std::shared_ptr<Branch> loop_branch = branch->getLoopBranch();
    std::shared_ptr<Branch> body_branch = branch->getBodyBranch();

    std::string true_label = build_unique_label();
    std::string false_label = build_unique_label();
    std::string loop_label = build_unique_label();

    // Handle the init branch.
    handle_stmt(init_branch);

    // This is the label where it will jump if the expression is true
    make_exact_label(loop_label);

    // Make the condition branches expression
    make_expression(cond_branch);

    // Handle the compare expression
    handle_compare_expression();

    // AX now contains true or false 

    do_asm("cmp ax, 0");
    do_asm("je " + false_label);

    // This is where we will jump if its true
    make_exact_label(true_label);

    // Handle the "FOR" statements body.
    handle_body(body_branch);

    // Now handle the loop
    handle_stmt(loop_branch);

    do_asm("jmp " + loop_label);

    // This is where we will jump if its false, the body will never be run.
    make_exact_label(false_label);


}

int CodeGen8086::getSizeOfVariableBranch(std::shared_ptr<VDEFBranch> vdef_branch)
{
    std::shared_ptr<Branch> data_type_branch = vdef_branch->getDataTypeBranch();
    std::shared_ptr<VarIdentifierBranch> var_identifier_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(vdef_branch->getVariableIdentifierBranch());

    int var_size;
    if (vdef_branch->getType() == "STRUCT_DEF")
    {
        var_size = getStructSize(data_type_branch->getValue());
    }
    else
    {
        // Variable sizes are 2 for pointers in the 8086 architecture
        if (vdef_branch->isPointer())
        {
            var_size = 2;
        }
        else
        {
            var_size = compiler->getDataTypeSize(data_type_branch->getValue());
        }

        // Check to see if we have an array going on here
        if (var_identifier_branch->hasRootArrayIndexBranch())
        {
            // We need to multiply all of the array indexes together so we know how many elements we have
            std::shared_ptr<ArrayIndexBranch> array_index_branch = std::dynamic_pointer_cast<ArrayIndexBranch>(var_identifier_branch->getRootArrayIndexBranch());
            int total_array_indexes = stoi(array_index_branch->getValueBranch()->getValue());
            while (array_index_branch->hasNextArrayIndexBranch())
            {
                array_index_branch = std::dynamic_pointer_cast<ArrayIndexBranch>(array_index_branch->getNextArrayIndexBranch());
                total_array_indexes *= stoi(array_index_branch->getValueBranch()->getValue());
            }

            // Calculate the array size
            var_size = total_array_indexes * var_size;
        }

    }

    return var_size;
}

int CodeGen8086::getFunctionArgumentIndex(std::shared_ptr<Branch> var_branch)
{
    std::string var_name;

    std::shared_ptr<VarIdentifierBranch> identifier_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
    var_name = identifier_branch->getVariableNameBranch()->getValue();


    for (int i = 0; i < this->func_arguments.size(); i++)
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(this->func_arguments.at(i));
        std::string vdef_var_name = vdef_branch->getNameBranch()->getValue();
        if (vdef_var_name == var_name)
        {
            return i;
        }
    }

    return -1;
}

int CodeGen8086::getBPOffsetForArgument(std::shared_ptr<Branch> var_branch)
{
    // * 2 as stack elements are 16 bits wide, +4 because this is where first element begins
    return (getFunctionArgumentIndex(var_branch) * 2) + 4;
}

std::shared_ptr<STRUCTBranch> CodeGen8086::getStructure(std::string struct_name)
{
    for (std::shared_ptr<Branch> branch : this->structures)
    {
        std::shared_ptr<STRUCTBranch> struct_branch = std::dynamic_pointer_cast<STRUCTBranch>(branch);
        std::shared_ptr<Branch> struct_name_branch = struct_branch->getStructNameBranch();
        if (struct_name_branch->getValue() == struct_name)
        {
            return struct_branch;
        }
    }

    return NULL;
}

std::shared_ptr<STRUCTBranch> CodeGen8086::getStructureFromScopeVariable(std::shared_ptr<Branch> branch)
{
    std::shared_ptr<Branch> scope_var = getScopeVariable(branch);
    std::shared_ptr<STRUCTDEFBranch> struct_def = std::dynamic_pointer_cast<STRUCTDEFBranch>(scope_var);
    std::shared_ptr<Branch> struct_def_name_branch = struct_def->getDataTypeBranch();
    for (int i = 0; i < this->structures.size(); i++)
    {
        std::shared_ptr<STRUCTBranch> structure = this->structures.at(i);
        std::shared_ptr<Branch> struct_name_branch = structure->getStructNameBranch();
        if (struct_def_name_branch->getValue() == struct_name_branch->getValue())
        {
            return structure;
        }
    }

    return NULL;
}

std::shared_ptr<Branch> CodeGen8086::getVariableFromStructure(std::shared_ptr<STRUCTBranch> structure, std::string var_name)
{
    for (std::shared_ptr<Branch> branch : structure->getStructBodyBranch()->getChildren())
    {
        if (branch->getType() == "V_DEF" ||
                branch->getType() == "STRUCT_DEF" ||
                branch->getType() == "V_DEF_PTR")
        {
            std::shared_ptr<VDEFBranch> v_def_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
            if (v_def_branch->getNameBranch()->getValue() == var_name)
            {
                return v_def_branch;
            }
        }
    }

    return NULL;
}

int CodeGen8086::getStructureVariableOffset(std::string struct_name, std::string var_name)
{
    std::shared_ptr<STRUCTBranch> structure_branch = getStructure(struct_name);
    return getStructureVariableOffset(structure_branch, var_name);
}

int CodeGen8086::getStructureVariableOffset(std::shared_ptr<STRUCTBranch> struct_branch, std::string var_name)
{
    int size = 0;
    std::shared_ptr<Branch> structure_body_branch = struct_branch->getStructBodyBranch();
    for (std::shared_ptr<Branch> branch : structure_body_branch->getChildren())
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        std::shared_ptr<Branch> data_type_branch = vdef_branch->getDataTypeBranch();
        std::shared_ptr<Branch> name_branch = vdef_branch->getNameBranch();
        if (name_branch->getValue() == var_name)
        {
            // We are done :)
            break;
        }

        if (vdef_branch->getType() == "STRUCT_DEF")
        {
            size += getStructSize(data_type_branch->getValue());
        }
        else
        {
            size += compiler->getDataTypeSize(data_type_branch->getValue());
        }
    }

    return size;
}

int CodeGen8086::getPosForStructureVariable(std::shared_ptr<Branch> branch)
{
    std::shared_ptr<Branch> left = branch->getFirstChild();
    std::shared_ptr<Branch> right = branch->getSecondChild();
    std::shared_ptr<STRUCTBranch> structure = NULL;

    int offset = 0;
    if (left->getType() == "STRUCT_ACCESS")
    {
        offset = getPosForStructureVariable(left);
        structure = this->last_structure;
        if (structure != NULL)
        {
            offset += getStructureVariableOffset(structure, right->getValue());
        }
    }
    else
    {
        structure = getStructureFromScopeVariable(left);
        offset = getStructureVariableOffset(structure, right->getValue());

        // Is the right branch also accessing a structure?
        std::shared_ptr<Branch> child = getVariableFromStructure(structure, right->getValue());
        if (child != NULL && child->getType() == "STRUCT_DEF")
        {
            std::shared_ptr<STRUCTDEFBranch> struct_def = std::dynamic_pointer_cast<STRUCTDEFBranch>(child);
            this->last_structure = getStructure(struct_def->getDataTypeBranch()->getValue());
        }
    }

    return offset;
}

int CodeGen8086::getStructSize(std::string struct_name)
{
    int size = 0;
    std::shared_ptr<STRUCTBranch> structure_branch = getStructure(struct_name);
    std::shared_ptr<Branch> structure_body_branch = structure_branch->getStructBodyBranch();
    for (std::shared_ptr<Branch> branch : structure_body_branch->getChildren())
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        std::shared_ptr<Branch> data_type_branch = vdef_branch->getDataTypeBranch();
        if (vdef_branch->getType() == "STRUCT_DEF")
        {
            size += getStructSize(data_type_branch->getValue());
        }
        else
        {
            size += compiler->getDataTypeSize(data_type_branch->getValue());
        }
    }

    return size;
}

int CodeGen8086::getBPOffsetForScopeVariable(std::shared_ptr<Branch> var_branch)
{
    std::shared_ptr<STRUCTAccessBranch> access_branch = NULL;
    std::string var_name;
    if (var_branch->getType() == "STRUCT_ACCESS")
    {
        access_branch = std::dynamic_pointer_cast<STRUCTAccessBranch>(var_branch);
        var_branch = access_branch->getDeepestAccessBranch()->getFirstChild();
    }
    else
    {
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
        var_name = var_iden_branch->getVariableNameBranch()->getValue();
    }

    int offset = 0;
    for (int i = 0; i < this->scope_variables.size(); i++)
    {
        std::shared_ptr<Branch> scope_var = this->scope_variables.at(i);
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(scope_var);
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(vdef_branch->getVariableIdentifierBranch());
        std::shared_ptr<Branch> name_branch = vdef_branch->getNameBranch();
        std::shared_ptr<Branch> data_type_branch = vdef_branch->getDataTypeBranch();

        int var_size = getSizeOfVariableBranch(vdef_branch);

        if (name_branch->getValue() == var_name)
        {

            /* If the data type is int16 or uint16 or a pointer then we need to add one more to it
             this is because of how we are storing the memory and the way the 8086 processor
             handles writes and reads to words, more can be read about this in the development diary
             on Day 48 at 21:33*/
            
            if (vdef_branch->isPointer() ||
                    data_type_branch->getValue() == "int16" ||
                    data_type_branch->getValue() == "uint16")
            {
                offset += 1;
            }

            /* If this is an array its important that the offset starts at the end of the array
             and not the beginning this is important so that accessing array elements through pointers
             will work as expected more is described in the diary on Day 48 at 22:59.*/
            
            if (var_iden_branch->hasRootArrayIndexBranch())
            {
                offset += var_size -1;
            }
            break;
        }

        offset += var_size;

    }

    // Are we doing a structure access? If so we need to add on the offset of the structure element we are accessing.
    if (access_branch != NULL)
    {
        offset += getPosForStructureVariable(access_branch);
    }

    return offset;
}

bool CodeGen8086::hasScopeVariable(std::shared_ptr<Branch> var_branch)
{
    std::string var_name;
    /* If this is a structure access then we need to get the first
     element as this element will be the scope variable name. E.g "a.b.c" "a" would be the scope variable name.*/
    if (var_branch->getType() == "STRUCT_ACCESS")
    {
        std::shared_ptr<STRUCTAccessBranch> access_branch = std::dynamic_pointer_cast<STRUCTAccessBranch>(var_branch);
        var_branch = access_branch->getDeepestAccessBranch()->getFirstChild();
    }
    else
    {
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
        var_name = var_iden_branch->getVariableNameBranch()->getValue();
    }

    for (int i = 0; i < this->scope_variables.size(); i++)
    {
        std::shared_ptr<Branch> branch = this->scope_variables.at(i);
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        std::shared_ptr<Branch> name_branch = vdef_branch->getNameBranch();
        if (name_branch->getValue() == var_name)
        {
            return true;
        }
    }

    return false;
}

bool CodeGen8086::hasArgumentVariable(std::shared_ptr<Branch> var_branch)
{
    return getFunctionArgumentIndex(var_branch) != -1;
}

int CodeGen8086::getVariableType(std::shared_ptr<Branch> var_branch)
{
    if (hasScopeVariable(var_branch))
        return SCOPE_VARIABLE;
    if (hasArgumentVariable(var_branch))
        return ARGUMENT_VARIABLE;

    return GLOBAL_VARIABLE;
}

int CodeGen8086::getSumOfScopeVariablesSizeSoFar()
{
    // Will need to be changed eventually due to structures
    // * 2 as scope variables currently use minimum and maximum of 2 bytes.
    return (this->scope_variables.size() * 2);
}

struct VARIABLE_ADDRESS CodeGen8086::getASMAddressForVariable(std::shared_ptr<Branch> var_branch)
{
    struct VARIABLE_ADDRESS address;
    address.var_type = getVariableType(var_branch);
    switch (address.var_type)
    {
    case ARGUMENT_VARIABLE:
        address.offset = getBPOffsetForArgument(var_branch);
        address.segment = "bp";
        address.op = "+";
        break;
    case SCOPE_VARIABLE:
        address.segment = "bp";
        address.op = "-";
        address.offset = getBPOffsetForScopeVariable(var_branch);
        break;
    }

    return address;


}

std::string CodeGen8086::getASMAddressForVariableFormatted(std::shared_ptr<Branch> var_branch)
{
    struct VARIABLE_ADDRESS address = getASMAddressForVariable(var_branch);
    return (address.segment + address.op + std::to_string(address.offset));
}

std::shared_ptr<Branch> CodeGen8086::getScopeVariable(std::shared_ptr<Branch> var_branch)
{
    /* If this is a structure access e.g "a.b.c.d" then the variable we need to return is "d"*/
    std::shared_ptr<STRUCTAccessBranch> access_branch = NULL;
    std::shared_ptr<Branch> var_name_branch = NULL;

    if (var_branch->getType() == "STRUCT_ACCESS")
    {
        access_branch = std::dynamic_pointer_cast<STRUCTAccessBranch>(var_branch);
        access_branch = std::dynamic_pointer_cast<STRUCTAccessBranch>(access_branch->getDeepestAccessBranch());
        var_name_branch = access_branch->getFirstChild();
    }
    else
    {
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
        var_name_branch = var_iden_branch->getVariableNameBranch();
    }

    for (std::shared_ptr<Branch> variable : this->scope_variables)
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(variable);
        std::shared_ptr<Branch> variable_name_branch = vdef_branch->getNameBranch();
        if (variable_name_branch->getValue() == var_name_branch->getValue())
        {
            // Is this structure access we are doing?
            if (access_branch != NULL)
            {
                std::shared_ptr<Branch> access_parent = NULL;
                std::shared_ptr<STRUCTBranch> struct_branch = getStructureFromScopeVariable(access_branch->getFirstChild());
                while (true)
                {
                    access_parent = access_branch->getParent();
                    std::shared_ptr<Branch> struct_var = getVariableFromStructure(struct_branch, access_branch->getSecondChild()->getValue());
                    if (access_parent->getType() == "STRUCT_ACCESS")
                    {
                        std::shared_ptr<STRUCTDEFBranch> struct_def = std::dynamic_pointer_cast<STRUCTDEFBranch>(struct_var);
                        struct_branch = getStructure(struct_def->getDataTypeBranch()->getValue());
                        access_branch = std::dynamic_pointer_cast<STRUCTAccessBranch>(access_parent);
                    }
                    else
                    {
                        return struct_var;
                    }
                }
            }

            return variable;
        }
    }

    return NULL;
}

std::shared_ptr<VDEFBranch> CodeGen8086::getVariable(std::shared_ptr<Branch> var_branch)
{
    std::shared_ptr<Branch> variable_branch = NULL;
    int var_type = getVariableType(var_branch);
    switch (var_type)
    {
    case ARGUMENT_VARIABLE:
        variable_branch = getFunctionArgumentVariable(var_branch);
        break;
    case SCOPE_VARIABLE:
        variable_branch = getScopeVariable(var_branch);
        break;
    }

    return std::dynamic_pointer_cast<VDEFBranch>(variable_branch);
}

std::shared_ptr<Branch> CodeGen8086::getFunctionArgumentVariable(std::shared_ptr<Branch> var_branch)
{
    for (std::shared_ptr<Branch> variable : this->func_arguments)
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(variable);
        std::shared_ptr<Branch> variable_name_branch = vdef_branch->getNameBranch();
        if (variable_name_branch->getValue() == var_branch->getValue())
        {
            return variable;
        }
    }

    return NULL;
}

bool CodeGen8086::isVariablePointer(std::shared_ptr<Branch> var_branch)
{
    return false;
}

bool CodeGen8086::is_cmp_logic_operator_nothing_or_and()
{
    return this->cmp_exp_last_logic_operator == "" || this->cmp_exp_last_logic_operator == "&&";
}

void CodeGen8086::generate_global_branch(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "V_DEF")
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        handle_global_var_def(vdef_branch);
    }
    else if (branch->getType() == "FUNC")
    {
        std::shared_ptr<FuncBranch> func_branch = std::dynamic_pointer_cast<FuncBranch>(branch);
        handle_function(func_branch);
    }
    else if (branch->getType() == "STRUCT")
    {
        std::shared_ptr<STRUCTBranch> struct_branch = std::dynamic_pointer_cast<STRUCTBranch>(branch);
        handle_structure(struct_branch);
    }
}

void CodeGen8086::assemble(std::string assembly)
{
    std::cout << assembly << std::endl;
}

std::shared_ptr<Linker> CodeGen8086::getLinker()
{
    return this->linker;
}