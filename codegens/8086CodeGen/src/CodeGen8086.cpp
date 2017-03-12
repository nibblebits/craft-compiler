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
 * File:   CodeGen8086.cpp
 * Author: Daniel McCarthy
 *
 * Created on 14 September 2016, 03:40
 * 
 * Description: 
 */

#include "CodeGen8086.h"
#include "Assembler8086.h"

CodeGen8086::CodeGen8086(Compiler* compiler, std::shared_ptr<VirtualObjectFormat> object_format) : CodeGenerator(compiler, object_format, "8086 CodeGenerator", POINTER_SIZE)
{
    this->compiler = compiler;
    this->current_label_index = 0;
    this->is_cmp_expression = false;
    this->do_signed = false;
    this->scope_size = 0;

    this->last_found_var_access_variable = NULL;
    this->cur_func = NULL;
    this->cur_func_scope_size = 0;
    this->breakable_label = "";
    this->continue_label = "";

    this->breakable_branch_to_stop_reset = NULL;
    this->continue_branch_to_stop_reset = NULL;

    // Setup a default label on the data segment for us to offset from for global variables
    make_label("data", "data");
}

CodeGen8086::~CodeGen8086()
{
}

struct formatted_segment CodeGen8086::format_segment(std::string segment_name)
{
    struct formatted_segment segment;

    // Commented for now as assembly simulator does not support it
    segment.start_segment = "segment " + segment_name;
    segment.end_segment = "; END SEGMENT";
    return segment;
}

void CodeGen8086::make_label(std::string label, std::string segment)
{
    do_asm("_" + label + ":", segment);
}

void CodeGen8086::make_exact_label(std::string label, std::string segment)
{
    do_asm(label + ":", segment);
}

std::string CodeGen8086::build_unique_label()
{
    std::string label_name = std::to_string(this->current_label_index);
    this->current_label_index++;
    return "_u" + label_name;
}

void CodeGen8086::setup_comparing()
{
    this->cmp_exp_false_label_name = build_unique_label();
    this->cmp_exp_end_label_name = build_unique_label();
    this->cmp_exp_true_label_name = build_unique_label();
    is_cmp_expression = true;
}

void CodeGen8086::new_breakable_label(std::shared_ptr<Branch> branch_to_stop_reset)
{
    // Save what is currently there if any
    if (this->breakable_label != "")
    {
        this->breakable_label_stack.push_back(this->breakable_label);
    }

    if (this->breakable_branch_to_stop_reset != NULL)
    {
        this->breakable_branch_to_stop_reset_stack.push_back(this->breakable_branch_to_stop_reset);
    }

    // Ok safe to overwrite
    this->breakable_label = build_unique_label();
    this->breakable_branch_to_stop_reset = branch_to_stop_reset;
}

void CodeGen8086::end_breakable_label()
{
    // Time to restore previous end label
    if (!this->breakable_label_stack.empty())
    {
        this->breakable_label = this->breakable_label_stack.back();
        this->breakable_label_stack.pop_back();
    }
    else
    {
        this->breakable_label = "";
    }

    if (!this->breakable_branch_to_stop_reset_stack.empty())
    {
        this->breakable_branch_to_stop_reset = this->breakable_branch_to_stop_reset_stack.back();
        this->breakable_branch_to_stop_reset_stack.pop_back();
    }
    else
    {
        this->breakable_branch_to_stop_reset = NULL;
    }
}

void CodeGen8086::new_continue_label(std::string label_name, std::shared_ptr<Branch> branch_to_stop_reset)
{
    // Save what is currently there if any
    if (this->continue_label != "")
    {
        this->continue_label_stack.push_back(this->continue_label);
    }

    if (this->continue_branch_to_stop_reset != NULL)
    {
        this->continue_branch_to_stop_reset_stack.push_back(this->continue_branch_to_stop_reset);
    }

    // Ok safe to overwrite
    this->continue_label = label_name;
    this->continue_branch_to_stop_reset = branch_to_stop_reset;
}

void CodeGen8086::end_continue_label()
{
    // Time to restore previous end label
    if (!this->continue_label_stack.empty())
    {
        this->continue_label = this->continue_label_stack.back();
        this->continue_label_stack.pop_back();
    }
    else
    {
        this->continue_label = "";
    }

    if (!this->continue_branch_to_stop_reset_stack.empty())
    {
        this->continue_branch_to_stop_reset = this->continue_branch_to_stop_reset_stack.back();
        this->continue_branch_to_stop_reset_stack.pop_back();
    }
    else
    {
        this->continue_branch_to_stop_reset = NULL;
    }
}

void CodeGen8086::new_scope(std::shared_ptr<StandardScopeBranch> scope_branch)
{
    // Backup the current scope if any
    if (this->current_scope != NULL)
    {
        this->current_scopes.push_back(this->current_scope);
    }

    // Now its safe to overwrite it
    this->current_scope = scope_branch;
}

void CodeGen8086::end_scope()
{
    if (!this->current_scopes.empty())
    {
        this->current_scope = this->current_scopes.back();
        this->current_scopes.pop_back();
    }
    else
    {
        this->current_scope = NULL;
    }
}

std::string CodeGen8086::make_unique_label(std::string segment)
{
    std::string label_name = build_unique_label();
    make_exact_label(label_name, segment);
    return label_name;
}

std::string CodeGen8086::make_string(std::shared_ptr<Branch> string_branch)
{
    std::string str = string_branch->getValue();
    std::string label_name = make_unique_label("data");
    do_asm("db '" + str + "', 0", "data");
    return label_name;
}

// The only supported inline assembly arguments are numbers and variables, you cannot use pointers either.

void CodeGen8086::make_inline_asm(struct stmt_info* s_info, std::shared_ptr<ASMBranch> asm_branch)
{
    std::string asm_str = asm_branch->getInstructionStartStringBranch()->getValue();
    for (std::shared_ptr<Branch> child_branch : asm_branch->getInstructionArgumentsBranch()->getChildren())
    {
        std::shared_ptr<ASMArgBranch> arg_child_branch = std::dynamic_pointer_cast<ASMArgBranch>(child_branch);
        std::shared_ptr<Branch> argument_val_branch = arg_child_branch->getArgumentValueBranch();
        std::string op_str;
        if (argument_val_branch->getType() == "VAR_IDENTIFIER")
        {
            std::shared_ptr<VarIdentifierBranch> arg_child_value_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(arg_child_branch->getArgumentValueBranch());
            op_str = getASMAddressForVariableFormatted(s_info, arg_child_value_branch);
        }
        else if (argument_val_branch->getType() == "number")
        {
            op_str = argument_val_branch->getValue();
        }

        asm_str += op_str + arg_child_branch->getNextStringBranch()->getValue();
    }

    do_asm(asm_str);
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

void CodeGen8086::make_mem_assignment(std::string dest, std::shared_ptr<Branch> value_exp, bool is_word, std::function<void() > assignment_val_processed)
{
    // Value is optional as it may have already been handled else where which is sometimes needed so registers don't get overwritten.
    if (value_exp != NULL)
    {
        // We have a value expression here so make it.
        make_expression(value_exp, NULL, assignment_val_processed);
    }

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

void CodeGen8086::handle_logical_expression(std::shared_ptr<Branch> exp_branch, struct stmt_info* s_info, bool should_setup)
{
    std::shared_ptr<Branch> left = exp_branch->getFirstChild();
    std::shared_ptr<Branch> right = exp_branch->getSecondChild();

    if (should_setup)
        setup_comparing();


    if (left->getType() != "E")
    {
        make_expression(left, s_info);
        // We must setup comparing as it would not have been done for this variable.
        make_compare_instruction(">", "ax", "0");
        // If we have an logical OR then we must end the compare expression here
        if (exp_branch->getValue() == "||")
        {
            handle_compare_expression();
        }
    }
    else if (compiler->isLogicalOperator(left->getValue()))
    {
        handle_logical_expression(left, s_info, false);
    }
    else
    {
        make_expression(left, s_info);
    }

    if (right->getType() != "E")
    {
        make_expression(right, s_info);
        make_compare_instruction(">", "ax", "0");
    }
    else if (compiler->isLogicalOperator(right->getValue()))
    {
        handle_logical_expression(right, s_info, false);
    }
    else
    {
        make_expression(right, s_info);
    }

    handle_compare_expression();
}

void CodeGen8086::make_expression(std::shared_ptr<Branch> exp, struct stmt_info* s_info, std::function<void() > exp_start_func, std::function<void() > exp_end_func)
{

    std::shared_ptr<Branch> left = NULL;
    std::shared_ptr<Branch> right = NULL;


    // Do we have something we need to notify about starting this expression?
    if (exp_start_func != NULL)
    {
        exp_start_func();
    }

    /* In cases where this happens "a && b" "a" and "b" have no operators of there own
 so we need to make the compare instruction ourself, we will check if the variable is above zero.*/
    if (compiler->isLogicalOperator(exp->getValue()))
    {
        handle_logical_expression(exp, s_info);
    }
    else
    {
        if (exp->getType() != "E")
        {
            make_expression_left(exp, "ax", s_info);
        }
        else
        {
            left = exp->getFirstChild();
            right = exp->getSecondChild();

            if (left->getType() == "E")
            {
                make_expression(left, s_info, NULL, NULL);
            }
            else if (right->getType() != "E")
            {
                make_expression_left(left, "ax", s_info);
            }

            // Save the AX register if we need to
            if (
                    left->getType() == "E" &&
                    right->getType() == "E")
            {
                do_asm("push ax");
            }

            if (right->getType() == "E")
            {
                make_expression(right, s_info);
                if (left->getType() != "E")
                {
                    make_expression_left(left, "cx", s_info);
                }
            }
            else
            {
                make_expression_right(right, s_info);
            }

            // Restore the AX register if we need to
            if (left->getType() == "E" &&
                    right->getType() == "E")
            {
                do_asm("pop cx");
            }


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
                    setup_comparing();
                }
            }


            make_math_instruction(exp->getValue(), "ax", "cx");

        }

        // Do we have something we need to notify about ending this expression?
        if (exp_end_func != NULL)
        {
            exp_end_func();
        }
    }
}

void CodeGen8086::make_expression_part(std::shared_ptr<Branch> exp, std::string register_to_store, struct stmt_info* s_info)
{
    if (exp->getType() == "number")
    {
        do_asm("mov " + register_to_store + ", " + exp->getValue());
    }
    else if (exp->getType() == "string")
    {
        std::string addr_to_str = make_string(exp);
        do_asm("mov " + register_to_store + ", " + addr_to_str);
    }
    else if (exp->getType() == "VAR_IDENTIFIER")
    {
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(exp);
        if (s_info->is_assigning_pointer && s_info->is_assignment_variable)
        {
            /* We don't really want to move the variable data into a register as this is the pointer assignment part of the statement 
             * its address is what we care about */

            // After the call to "make_var_access" the "BX" register should hold the address to the variable
            make_var_access(s_info, var_iden_branch);
        }
        else
        {
            // This is a variable so set register to store to the value of this variable
            make_move_reg_variable(register_to_store, var_iden_branch, s_info);
        }
    }
    else if (exp->getType() == "LOGICAL_NOT")
    {
        make_logical_not(std::dynamic_pointer_cast<LogicalNotBranch>(exp), register_to_store, s_info);
    }
    else if (exp->getType() == "PTR")
    {
        std::shared_ptr<PTRBranch> ptr_branch = std::dynamic_pointer_cast<PTRBranch>(exp);
        handle_ptr(s_info, ptr_branch);
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
        make_move_var_addr_to_reg(s_info, register_to_store, var_branch);
    }
    else if (exp->getType() == "ASSIGN")
    {
        // We have an assignment in this expression.
        std::shared_ptr<AssignBranch> assign_branch = std::dynamic_pointer_cast<AssignBranch>(exp);
        handle_scope_assignment(assign_branch);
    }
}

void CodeGen8086::make_expression_left(std::shared_ptr<Branch> exp, std::string register_to_store, struct stmt_info* s_info)
{
    /* Blank AH ready for 8 bit identifiers.
     * This is required due to data left in the AH register due to previous expressions,
     * I believe a better alternative can be thought of but for now this will do.
     */
    if (exp->getType() == "VAR_IDENTIFIER")
    {
        std::shared_ptr<VDEFBranch> vdef_branch = getVariable(exp);
        if (!vdef_branch->isPointer() && vdef_branch->getDataTypeBranch()->getDataTypeSize() == 1)
        {
            do_asm("xor ah, ah");
        }
    }
    make_expression_part(exp, register_to_store, s_info);
}

void CodeGen8086::make_expression_right(std::shared_ptr<Branch> exp, struct stmt_info* s_info)
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
        /* Blank CH ready for 8 bit identifiers.
         * This is required due to data left in the CH register due to previous expressions,
         * I believe a better alternative can be thought of but for now this will do.
         */
        if (exp->getType() == "VAR_IDENTIFIER")
        {
            std::shared_ptr<VDEFBranch> vdef_branch = getVariable(exp);
            if (vdef_branch->getDataTypeBranch()->getDataTypeSize() == 1)
            {
                do_asm("xor ch, ch");
            }
        }

        make_expression_part(exp, "cx", s_info);
    }

}

bool CodeGen8086::is_gen_reg_16_bit(std::string reg)
{
    return reg == "ax" ||
            reg == "bx" ||
            reg == "cx" ||
            reg == "dx";

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

        if (do_signed)
        {
            do_asm("imul " + first_reg);
        }
        else
        {
            do_asm("mul " + first_reg);
        }
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
        if (do_signed)
        {
            do_asm("idiv " + first_reg);
        }
        else
        {
            do_asm("div " + first_reg);
        }
        if (is_gen_reg_16_bit(first_reg))
        {
            // This is a 16 bit division so the DX register will contain the remainder, lets move it into the AX register
            do_asm("mov ax, dx");
        }
        else
        {
            // This is an 8 bit division so the AH register will contain the result, lets exchange them
            do_asm("xchg ah, al");
            // Finally erase the ah register.
            do_asm("xor ah, ah");
        }

    }
    else if (op == "%")
    {
        /*
         * If the first register is ax then set the first register to the second register as "div" instruction
         * uses ax as first reg regardless */
        if (first_reg == "ax")
        {
            first_reg = second_reg;
        }

        do_asm("div " + first_reg);
        if (is_gen_reg_16_bit(first_reg))
        {
            // This is a 16 bit division so the DX register will contain the remainder, lets move it into the AX register
            do_asm("mov ax, dx");
        }
        else
        {
            // This is an 8 bit division so the AH register will contain the result, lets exchange them
            do_asm("xchg ah, al");
            // Finally erase the ah register.
            do_asm("xor ah, ah");
        }
    }
    else if (op == "^")
    {
        do_asm("xor " + first_reg + ", " + second_reg);
    }
    else if (op == "|")
    {
        do_asm("or " + first_reg + ", " + second_reg);
    }
    else if (op == "&")
    {
        do_asm("and " + first_reg + ", " + second_reg);
    }
    else if (op == "<<")
    {
        // Only the CL register can be used as the second parameter so lets see if we need to move anything
        if (second_reg != "cx" &&
                second_reg != "cl")
        {
            // We need to move the total bits to shift into the CL register
            do_asm("mov cl, " + convert_full_reg_to_low_reg(second_reg));
        }
        do_asm("rcl " + first_reg + ", " + "cl");
    }
    else if (op == ">>")
    {
        // Only the CL register can be used as the second parameter so lets see if we need to move anything
        if (second_reg != "cx" &&
                second_reg != "cl")
        {
            // We need to move the total bits to shift into the CL register
            do_asm("mov cl, " + convert_full_reg_to_low_reg(second_reg));
        }
        do_asm("rcr " + first_reg + ", " + "cl");
    }
    else if (
            op == "!=" ||
            op == "==" ||
            op == "<=" ||
            op == ">=" ||
            op == ">" ||
            op == "<")
    {
        make_compare_instruction(op, first_reg, second_reg);
    }
    else
    {
        throw CodeGeneratorException("void CodeGen8086::make_math_instruction(std::string op): expecting a valid operator");
    }

    this->do_signed = false;
}

void CodeGen8086::make_compare_instruction(std::string op, std::string first_value, std::string second_value)
{
    // We must compare
    do_asm("cmp " + first_value + ", " + second_value);

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

void CodeGen8086::move_data_to_register(std::string reg, std::string pos, int data_size)
{
    do_asm("; MOVE DATA TO REGISTER");
    if (data_size == 1)
    {
        // We don't want anything left in the register so lets blank it
        do_asm("xor " + reg + ", " + reg);

        /* Bytes must use the lower end of the registers and not the full register
         * we must break it down here before continuing */
        reg = convert_full_reg_to_low_reg(reg);
    }

    do_asm("mov " + reg + ", " + "[" + pos + "]");
}

void CodeGen8086::dig_bx_to_address(int depth)
{
    for (int i = 0; i < depth; i++)
    {
        do_asm("mov bx, [bx]");
    }
}

void CodeGen8086::make_move_reg_variable(std::string reg, std::shared_ptr<VarIdentifierBranch> var_branch, struct stmt_info* s_info)
{
    std::shared_ptr<VDEFBranch> vdef_branch = var_branch->getVariableDefinitionBranch();
    std::shared_ptr<VarIdentifierBranch> vdef_var_iden_branch = vdef_branch->getVariableIdentifierBranch();
    if (vdef_branch->isSigned())
    {
        //  This variable is signed so mark it as so, so that signed appropriates will be used with operators.
        this->do_signed = true;
    }

    int data_size;
    std::string pos = make_var_access(s_info, var_branch, &data_size);

    // When accessing arrays without an index its address should be returned
    if (vdef_var_iden_branch->hasRootArrayIndexBranch() && !var_branch->getFinalVarIdentifierBranch()->hasRootArrayIndexBranch())
    {
        do_asm("lea " + reg + ", [" + pos + "]");
    }
    else
    {
        // We need to move the data from "pos" into the register
        move_data_to_register(reg, pos, data_size);
    }

}

void CodeGen8086::make_move_var_addr_to_reg(struct stmt_info* s_info, std::string reg_name, std::shared_ptr<VarIdentifierBranch> var_branch)
{
    std::string pos = make_var_access(s_info, var_branch);
    do_asm("lea " + reg_name + ", [" + pos + "]");
}

void CodeGen8086::make_array_offset_instructions(struct stmt_info* s_info, std::shared_ptr<ArrayIndexBranch> array_branch, int size_p_elem)
{
    std::shared_ptr<ArrayIndexBranch> current_branch = array_branch;

    int c = 0;
    while (true)
    {
        std::shared_ptr<Branch> value_branch = current_branch->getValueBranch();
        // Make the expression of the array index
        make_expression(value_branch, s_info);

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

    if (size_p_elem == 2)
    {
        do_asm("shl ax, 1");
    }
    else if (size_p_elem > 2)
    {
        do_asm("mov cx, " + std::to_string(size_p_elem));
        do_asm("mul cx");
    }
}

void CodeGen8086::make_move_mem_to_mem(VARIABLE_ADDRESS &dest_loc, VARIABLE_ADDRESS &from_loc, int size)
{
    std::string continue_lbl_name;
    do_asm("mov si, " + from_loc.segment);
    if (from_loc.op == "+")
    {
        do_asm("add si, " + std::to_string(from_loc.offset));
    }
    else if (from_loc.op == "-")
    {
        do_asm("sub si, " + std::to_string(from_loc.offset));
    }

    do_asm("mov di, " + dest_loc.segment);
    if (dest_loc.op == "+")
    {
        do_asm("add si, " + std::to_string(dest_loc.offset));
    }
    else if (dest_loc.op == "-")
    {
        do_asm("sub si, " + std::to_string(dest_loc.offset));
    }

    do_asm("mov cx, " + std::to_string(size));
    continue_lbl_name = make_unique_label();
    do_asm("lodsb");
    do_asm("stosb");
    do_asm("loop " + continue_lbl_name);
}

void CodeGen8086::make_move_mem_to_mem(std::string dest_loc, std::string from_loc, int size)
{
    do_asm("; Moving memory at address " + from_loc + " to " + dest_loc + ", size: " + std::to_string(size));
    std::string continue_lbl_name;
    do_asm("lea si, " + from_loc);
    do_asm("lea di, " + dest_loc);
    do_asm("mov cx, " + std::to_string(size));
    continue_lbl_name = make_unique_label();
    do_asm("lodsb");
    do_asm("stosb");
    do_asm("loop " + continue_lbl_name);
}

void CodeGen8086::make_var_access_rel_base(struct stmt_info* s_info, std::shared_ptr<VarIdentifierBranch> var_branch, std::shared_ptr<VDEFBranch>* vdef_in_question_branch, std::shared_ptr<VarIdentifierBranch>* var_access_iden_branch, std::string base_reg, std::shared_ptr<STRUCTBranch> current_struct)
{

    std::shared_ptr<VarIdentifierBranch> root_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
    std::shared_ptr<Branch> root_iden_name_branch = root_iden_branch->getVariableNameBranch();
    std::shared_ptr<VDEFBranch> root_vdef_branch;

    // No structure so the variable we are looking for will be directly accessible
    if (current_struct == NULL)
    {
        root_vdef_branch = getVariable(root_iden_branch);
    }
    else
    {
        // Ok we have a structure so we need to instead get the structure 
        root_vdef_branch = getVariableFromStructure(current_struct, root_iden_name_branch->getValue());
        // We are currently in a structure so lets add on the offset to the variable identified.
        int offset = getStructureVariableOffset(current_struct, root_iden_name_branch->getValue());
        do_asm("add " + base_reg + ", " + std::to_string(offset));
    }

    if (root_iden_branch->hasRootArrayIndexBranch())
    {
        make_array_offset_instructions(s_info, root_iden_branch->getRootArrayIndexBranch(), root_vdef_branch->getDataTypeBranch()->getDataTypeSize(true));
    }


    // Store the last found VDEF variable, we will need to use this later.
    if (vdef_in_question_branch != NULL)
        *vdef_in_question_branch = root_vdef_branch;

    // Store the last found access var identifier variable, we will need to use this later
    if (var_access_iden_branch != NULL)
        *var_access_iden_branch = root_iden_branch;

    std::shared_ptr<Branch> root_vdef_data_type_branch = root_vdef_branch->getDataTypeBranch();
    if (root_iden_branch->hasStructureAccessBranch())
    {
        std::shared_ptr<STRUCTBranch> structure = getStructure(root_vdef_data_type_branch->getValue());
        make_var_access_rel_base(
                                 s_info,
                                 std::dynamic_pointer_cast<VarIdentifierBranch>(root_iden_branch->getStructureAccessBranch()->getFirstChild()),
                                 vdef_in_question_branch,
                                 var_access_iden_branch,
                                 base_reg,
                                 structure
                                 );
    }

}

void CodeGen8086::handle_struct_access(struct stmt_info* s_info, std::shared_ptr<STRUCTAccessBranch> struct_access_branch)
{
    std::string pos = std::to_string(struct_access_branch->getVarIdentifierBranch()->getVariableDefinitionBranch(true)->getPositionRelScope());
    do_asm("mov bx, [bx+" + pos + "]");
}

std::string CodeGen8086::make_var_access(struct stmt_info* s_info, std::shared_ptr<VarIdentifierBranch> var_branch, int* data_size)
{
    std::string pos = "";
    pos = getASMAddressForVariableFormatted(s_info, var_branch);
    if (data_size != NULL)
        *data_size = var_branch->getVariableDefinitionBranch()->getDataTypeBranch()->getDataTypeSize();

    return pos;
}

void CodeGen8086::make_appendment(std::string target_reg, std::string op, std::string pos)
{
    // Load the old value
    do_asm("mov " + target_reg + ", [" + pos + "]");
    if (op == "+=")
    {
        do_asm("add " + target_reg + ", ax");
    }
    else if (op == "-=")
    {
        do_asm("sub " + target_reg + ", ax");
    }
    else if (op == "*=")
    {
        do_asm("mul " + target_reg + ", ax");
    }
    else if (op == "/=")
    {
        do_asm("div " + target_reg + ", ax");
    }
    else
    {
        throw Exception("Appendment operator \"" + op + "\" is not implemented.", "void CodeGen8086::make_appendment(std::string target_reg, std::string op, std::string pos)");
    }
}

void CodeGen8086::make_var_assignment(std::shared_ptr<Branch> var_branch, std::shared_ptr<Branch> value, std::string op)
{
    struct stmt_info s_info;
    s_info.is_assignment = true;

    bool is_word;
    std::string pos;
    if (var_branch->getType() == "PTR")
    {
        std::shared_ptr<PTRBranch> ptr_branch = std::dynamic_pointer_cast<PTRBranch>(var_branch);
        s_info.is_assigning_pointer = true;

        s_info.is_assignment_variable = true;
        handle_ptr(&s_info, ptr_branch);
        is_word = s_info.assignment_data_size == 2;
        pos = s_info.pointer_var_position;
        s_info.is_assignment_variable = false;

        // Make the value expression
        make_expression(value, &s_info);

        // Is this assignment an appendment
        if (op != "=")
        {
            // Ok this is an appendment so we need to adjust the value before setting it again
            make_appendment("dx", op, "bx");
            // Overwrite AX with the appended value
            do_asm("mov ax, dx");
        }

        make_mem_assignment(pos, NULL, is_word, NULL);
    }
    else
    {

        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
        std::shared_ptr<VDEFBranch> vdef_in_question_branch = var_iden_branch->getVariableDefinitionBranch();

        // Are we assigning a pointer?
        if (vdef_in_question_branch->isPointer())
        {
            s_info.is_assigning_pointer = true;
        }

        int data_size;
        s_info.is_assignment_variable = true;
        pos = make_var_access(&s_info, var_iden_branch, &data_size);
        s_info.is_assignment_variable = false;

        is_word = data_size == 2;

        // Make the value expression
        make_expression(value, &s_info);

        // Is this assignment an appendment
        if (op != "=")
        {
            // Ok this is an appendment so we need to adjust the value before setting it again
            make_appendment("dx", op, pos);
            // Overwrite AX with the appended value
            do_asm("mov ax, dx");
        }

        // This is a primitive type assignment, including pointer assignments

        make_mem_assignment(pos, NULL, is_word, NULL);

    }
}

void CodeGen8086::make_logical_not(std::shared_ptr<LogicalNotBranch> logical_not_branch, std::string register_to_store, struct stmt_info* s_info)
{
    make_expression(logical_not_branch->getSubjectBranch(), s_info);
    std::string is_zero_lbl = build_unique_label();
    std::string end_label = build_unique_label();
    do_asm("test " + register_to_store + ", " + register_to_store);
    do_asm("je " + is_zero_lbl);
    do_asm("xor " + register_to_store + ", " + register_to_store);
    do_asm("jmp " + end_label);
    make_exact_label(is_zero_lbl);
    do_asm("mov " + register_to_store + ", 1");
    make_exact_label(end_label);
}

void CodeGen8086::calculate_scope_size(std::shared_ptr<ScopeBranch> scope_branch)
{
    this->scope_size = scope_branch->getScopeSize();

    if (scope_branch->getType() == "FOR")
    {
        // This is a FOR branch so we also want to include its BODY's scope size
        std::shared_ptr<FORBranch> for_branch = std::dynamic_pointer_cast<FORBranch>(scope_branch);
        this->scope_size += for_branch->getBodyBranch()->getScopeSize();
    }
    // Generate some ASM to reserve space on the stack for this scope
    do_asm("sub sp, " + std::to_string(this->scope_size));

    current_scopes_sizes.push_back(this->scope_size);

}

void CodeGen8086::reset_scope_size()
{
    // Add the stack pointer by the scope size so the memory is recycled.
    do_asm("add sp, " + std::to_string(this->scope_size));
    current_scopes_sizes.pop_back();

    if (current_scopes_sizes.empty())
    {
        this->scope_size = 0;
    }
    else
    {
        this->scope_size = current_scopes_sizes.back();
    }
}

void CodeGen8086::handle_ptr(struct stmt_info* s_info, std::shared_ptr<PTRBranch> ptr_branch)
{
    do_asm("; POINTER HANDLING ");

    // We need to set the is child of pointer flag ready for any children we are about to process
    s_info->is_child_of_pointer = true;
    s_info->pointer_your_child_of = ptr_branch;

    std::shared_ptr<Branch> exp_branch = ptr_branch->getExpressionBranch();
    make_expression(exp_branch, s_info, NULL, NULL);

    s_info->is_child_of_pointer = false;
}

void CodeGen8086::handle_global_var_def(std::shared_ptr<VDEFBranch> vdef_branch)
{
    // Push this variable definition to the global variables vector
    this->global_variables.push_back(vdef_branch);

    // Handle the variable declaration
    std::shared_ptr<VarIdentifierBranch> variable_iden_branch = vdef_branch->getVariableIdentifierBranch();
    std::shared_ptr<Branch> variable_data_type_branch = vdef_branch->getDataTypeBranch();
    std::shared_ptr<Branch> variable_name_branch = variable_iden_branch->getVariableNameBranch();


    // E.g "rb", "dw", or "db"
    std::string data_write_macro;
    // The value for this e.g "dw VALUE"
    std::string data_write_macro_value = "0";

    // Value branch may only be a number for global variables, the framework will ensure this for us no need to check
    std::shared_ptr<Branch> value_branch = NULL;
    if (vdef_branch->hasValueExpBranch())
    {
        value_branch = vdef_branch->getValueExpBranch();
    }

    make_label(variable_name_branch->getValue(), "data");

    if (vdef_branch->getType() == "STRUCT_DEF"
            && !vdef_branch->isPointer())
    {
        int struct_size = getSizeOfVariableBranch(vdef_branch);
        data_write_macro = "rb";
        data_write_macro_value = std::to_string(struct_size);
    }
    else
    {
        if (vdef_branch->getDataTypeBranch()->isPointer() || vdef_branch->getDataTypeBranch()->getDataTypeSize() == 2)
        {
            data_write_macro = "dw";
        }
        else
        {
            data_write_macro = "db";
        }

        if (value_branch != NULL)
        {
            data_write_macro_value = value_branch->getValue();
        }
    }


    do_asm(data_write_macro + " " + data_write_macro_value, "data");

}

void CodeGen8086::handle_structure(std::shared_ptr<STRUCTBranch> struct_branch)
{
    this->structures.push_back(struct_branch);
}

void CodeGen8086::handle_function_definition(std::shared_ptr<FuncDefBranch> func_def_branch)
{
    std::shared_ptr<Branch> name_branch = func_def_branch->getNameBranch();
    do_asm("extern _" + name_branch->getValue());
}

void CodeGen8086::handle_function(std::shared_ptr<FuncBranch> func_branch)
{
    struct stmt_info s_info;

    // Clear previous scope variables from other functions
    this->scope_variables.clear();

    std::shared_ptr<Branch> name_branch = func_branch->getNameBranch();
    std::shared_ptr<Branch> arguments_branch = func_branch->getArgumentsBranch();
    std::shared_ptr<BODYBranch> body_branch = func_branch->getBodyBranch();

    // Make the function global.
    // Commented for now as assembly simulator does not support it
    do_asm(";global " + name_branch->getValue());

    // Make the function label
    make_label(name_branch->getValue());

    this->cur_func = func_branch;
    this->cur_func_scope_size = body_branch->getScopeSize();

    do_asm("push bp");
    do_asm("mov bp, sp");
    // Generate some ASM to reserve space on the stack for this scope
    do_asm("sub sp, " + std::to_string(this->cur_func_scope_size));

    // Handle the arguments
    handle_func_args(arguments_branch);

    // Handle the body
    handle_body(&s_info, body_branch);


}

void CodeGen8086::handle_func_args(std::shared_ptr<Branch> arguments)
{
    this->func_arguments.clear();
    for (std::shared_ptr<Branch> arg : arguments->getChildren())
    {
        std::shared_ptr<VDEFBranch> arg_vdef = std::dynamic_pointer_cast<VDEFBranch>(arg);
        if (!arg_vdef->isPointer())
        {
            // All function arguments are 2 bytes in size.
            arg_vdef->getDataTypeBranch()->setCustomDataTypeSize(getPointerSize());
        }
        this->func_arguments.push_back(arg_vdef);
    }
}

void CodeGen8086::handle_body(struct stmt_info* s_info, std::shared_ptr<Branch> body)
{
    new_scope(std::dynamic_pointer_cast<StandardScopeBranch>(body));
    // Now we can handle every statement
    for (std::shared_ptr<Branch> stmt : body->getChildren())
    {
        handle_stmt(s_info, stmt);
    }
    end_scope();
}

void CodeGen8086::handle_stmt(struct stmt_info* s_info, std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "ASSIGN")
    {
        std::shared_ptr<AssignBranch> assign_branch = std::dynamic_pointer_cast<AssignBranch>(branch);
        handle_scope_assignment(assign_branch);
    }
    else if (branch->getType() == "ASM")
    {
        std::shared_ptr<ASMBranch> asm_branch = std::dynamic_pointer_cast<ASMBranch>(branch);
        make_inline_asm(s_info, asm_branch);
    }
    else if (branch->getType() == "FUNC_CALL")
    {
        std::shared_ptr<FuncCallBranch> func_call_branch = std::dynamic_pointer_cast<FuncCallBranch>(branch);
        handle_function_call(func_call_branch);
    }
    else if (branch->getType() == "RETURN")
    {
        handle_func_return(s_info, std::dynamic_pointer_cast<ReturnBranch>(branch));
    }
    else if (branch->getType() == "V_DEF" ||
            branch->getType() == "STRUCT_DEF")
    {
        handle_scope_variable_declaration(std::dynamic_pointer_cast<VDEFBranch>(branch));
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
    else if (branch->getType() == "BREAK")
    {
        std::shared_ptr<BreakBranch> break_branch = std::dynamic_pointer_cast<BreakBranch>(branch);
        handle_break(break_branch);
    }
    else if (branch->getType() == "CONTINUE")
    {
        std::shared_ptr<ContinueBranch> continue_branch = std::dynamic_pointer_cast<ContinueBranch>(branch);
        handle_continue(continue_branch);
    }
    else if (branch->getType() == "WHILE")
    {
        std::shared_ptr<WhileBranch> while_branch = std::dynamic_pointer_cast<WhileBranch>(branch);
        handle_while_stmt(while_branch);
    }
}

void CodeGen8086::handle_function_call(std::shared_ptr<FuncCallBranch> branch)
{
    struct stmt_info s_info;

    std::shared_ptr<Branch> func_name_branch = branch->getFuncNameBranch();
    std::shared_ptr<Branch> func_params_branch = branch->getFuncParamsBranch();

    std::vector<std::shared_ptr < Branch>> params = func_params_branch->getChildren();

    // Parameters are treated as an expression, they must be pushed on backwards due to how the stack works
    for (int i = params.size() - 1; i >= 0; i--)
    {
        std::shared_ptr<Branch> param = params.at(i);
        make_expression(param, &s_info);
        // Push the expression to the stack as this is a function call
        do_asm("push ax");
    }

    // Now call the function :)
    do_asm("call _" + func_name_branch->getValue());

    /* Restore the stack pointer to what it was to recycle the memory */
    do_asm("add sp, " + std::to_string(params.size() * 2));
}

void CodeGen8086::handle_scope_assignment(std::shared_ptr<AssignBranch> assign_branch)
{
    std::shared_ptr<Branch> var_to_assign_branch = assign_branch->getVariableToAssignBranch();
    std::shared_ptr<Branch> value = assign_branch->getValueBranch();

    make_var_assignment(var_to_assign_branch, value, assign_branch->getOperator());

}

void CodeGen8086::handle_func_return(struct stmt_info* s_info, std::shared_ptr<ReturnBranch> return_branch)
{
    if (return_branch->hasExpressionBranch())
    {
        // We have something to return
        // AX will be set to the value to return once the expression is complete.
        make_expression(return_branch->getExpressionBranch(), s_info);
    }

    // Restore the stack pointer
    std::shared_ptr<Branch> branch_to_stop = cur_func->getArgumentsBranch();
    do_asm("add sp, " + std::to_string(this->current_scope->getScopeSize(GET_SCOPE_SIZE_INCLUDE_PARENT_SCOPES,
                                                                         [&](std::shared_ptr<Branch> branch) -> bool
                                                                         {
                                                                             // We should stop at the function arguments so it doesn't include any more parent scopes when it reaches this point
                                                                             if (branch == branch_to_stop)
                                                                             {
                                                                                 return false;
                                                                             }

                                                                             return true;
                                                                         })));

    // Pop from the stack back to the BP(Base Pointer) now we are leaving this function
    do_asm("pop bp");
    do_asm("ret");
}

void CodeGen8086::handle_compare_expression()
{
    /* We must first check that this is not a single compare, if a single value is passed with no operator 
     * then it will evaluate to true if the value is above zero otherwise false.*/
    if (!this->is_cmp_expression)
    {
        setup_comparing();
        do_asm("cmp ax, 0");
        do_asm("je " + this->cmp_exp_false_label_name);
    }


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

void CodeGen8086::handle_scope_variable_declaration(std::shared_ptr<VDEFBranch> def_branch)
{
    // Register a scope variable
    this->scope_variables.push_back(def_branch);

    // Handle the variable declaration
    std::shared_ptr<Branch> variable_branch = def_branch->getVariableIdentifierBranch();
    if (def_branch->hasValueExpBranch())
    {
        std::shared_ptr<Branch> value_branch = def_branch->getValueExpBranch();
        make_var_assignment(variable_branch, value_branch, "=");
    }
}

void CodeGen8086::handle_if_stmt(std::shared_ptr<IFBranch> branch)
{
    struct stmt_info s_info;

    std::shared_ptr<Branch> exp_branch = branch->getExpressionBranch();
    std::shared_ptr<BODYBranch> body_branch = branch->getBodyBranch();


    // Process the expression of the "IF" statement
    make_expression(exp_branch, &s_info);

    // Handle the compare expression
    handle_compare_expression();

    // AX now contains true or false 
    std::string true_label = build_unique_label();
    std::string false_label = build_unique_label();
    std::string end_label = build_unique_label();

    do_asm("cmp ax, 0");
    do_asm("je " + false_label);
    // This is where we will jump if its true
    make_exact_label(true_label);

    calculate_scope_size(body_branch);

    // Handle the "IF" statements body.
    handle_body(&s_info, body_branch);

    reset_scope_size();

    // Ok lets jump over the false label.
    do_asm("jmp " + end_label);

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
        handle_body(&s_info, else_body_branch);
    }


    // This is the end of the if statement
    make_exact_label(end_label);
}

void CodeGen8086::handle_for_stmt(std::shared_ptr<FORBranch> branch)
{
    struct stmt_info s_info;

    do_asm("; FOR STATEMENT");
    std::shared_ptr<Branch> init_branch = branch->getInitBranch();
    std::shared_ptr<Branch> cond_branch = branch->getCondBranch();
    std::shared_ptr<Branch> loop_branch = branch->getLoopBranch();
    std::shared_ptr<Branch> body_branch = branch->getBodyBranch();

    std::string true_label = build_unique_label();
    std::string false_label = build_unique_label();
    std::string loop_label = build_unique_label();
    // The part of the loop that may modify a variable
    std::string loop_part_label = build_unique_label();

    // Setup the new breakable label so that break statements can break out of this for loop.
    new_breakable_label(branch);

    // Set the new continue label to be the loop label, so that if the programmer uses continue it will jump to the loop label.
    new_continue_label(loop_part_label, body_branch);

    // Calculate the scope size for the "for" loop
    calculate_scope_size(std::dynamic_pointer_cast<ScopeBranch>(branch));

    // Handle the init branch.
    handle_stmt(&s_info, init_branch);

    // This is the label where it will jump if the expression is true
    make_exact_label(loop_label);

    // Make the condition branches expression
    make_expression(cond_branch, &s_info);

    // Handle the compare expression
    handle_compare_expression();

    // AX now contains true or false 

    do_asm("cmp ax, 0");
    do_asm("je " + false_label);

    // This is where we will jump if its true
    make_exact_label(true_label);

    // Handle the "FOR" statements body.
    handle_body(&s_info, body_branch);

    // Make the label where the actual loop logic is preformed this is where the variable will be modified
    make_exact_label(loop_part_label);

    // Now handle the loop
    handle_stmt(&s_info, loop_branch);

    do_asm("jmp " + loop_label);

    // This is where we will jump if its false, the body will never be run.
    make_exact_label(false_label);

    // Lets write the breakable label
    make_exact_label(breakable_label);

    // We are done with the breakable label.
    end_breakable_label();

    // We are done with the continue label.
    end_continue_label();

    reset_scope_size();

}

void CodeGen8086::handle_while_stmt(std::shared_ptr<WhileBranch> branch)
{
    struct stmt_info s_info;
    std::shared_ptr<Branch> exp_branch = branch->getExpressionBranch();
    std::shared_ptr<BODYBranch> body_branch = branch->getBodyBranch();

    std::string exp_label = build_unique_label();
    std::string true_label = build_unique_label();
    std::string false_label = build_unique_label();

    // We need to create a new breakable label as you can break from "WHILE" statements
    new_breakable_label(body_branch);

    // We need to setup expression labels for the continue label.
    new_continue_label(exp_label, body_branch);

    make_exact_label(exp_label);

    // Process the expression of the "WHILE" statement
    make_expression(exp_branch, &s_info);

    // Handle the compare expression
    handle_compare_expression();

    // AX now contains true or false 

    do_asm("cmp ax, 0");
    do_asm("je " + false_label);

    calculate_scope_size(body_branch);

    // This is where we will jump if its true
    make_exact_label(true_label);

    // Handle the "WHILE" statements body.
    handle_body(&s_info, body_branch);

    reset_scope_size();

    // The program will jump back to the expression label in order to see if it still applies
    do_asm("jmp " + exp_label);

    // This is where we will jump if its false, the body will never be run.
    make_exact_label(false_label);

    // Lets make the label for where we end up if we break
    make_exact_label(this->breakable_label);

    end_breakable_label();

    end_continue_label();
}

void CodeGen8086::handle_break(std::shared_ptr<BreakBranch> branch)
{
    do_asm("; BREAK");
    // Looks like we are breaking out of this
    std::shared_ptr<Branch> branch_to_stop = this->breakable_branch_to_stop_reset;
    do_asm("add sp, " + std::to_string(branch->getLocalScope()->getScopeSize(GET_SCOPE_SIZE_INCLUDE_PARENT_SCOPES, NULL,
                                                                             [&](std::shared_ptr<Branch> branch) -> bool
                                                                             {
                                                                                 // We should stop at the function arguments so it doesn't include any more parent scopes when it reaches this point
                                                                                 if (branch == branch_to_stop)
                                                                                 {
                                                                                     return false;
                                                                                 }

                                                                                 return true;
                                                                             })));
    do_asm("jmp " + this->breakable_label);
}

void CodeGen8086::handle_continue(std::shared_ptr<ContinueBranch> branch)
{
    std::shared_ptr<Branch> branch_to_stop = this->continue_branch_to_stop_reset;
    do_asm("add sp, " + std::to_string(branch->getLocalScope()->getScopeSize(GET_SCOPE_SIZE_INCLUDE_PARENT_SCOPES, NULL,
                                                                             [&](std::shared_ptr<Branch> branch) -> bool
                                                                             {
                                                                                 // We should stop at the function arguments so it doesn't include any more parent scopes when it reaches this point
                                                                                 if (branch == branch_to_stop)
                                                                                 {
                                                                                     return false;
                                                                                 }

                                                                                 return true;
                                                                             })));
    do_asm("jmp " + this->continue_label);
}

void CodeGen8086::handle_array_index(struct stmt_info* s_info, std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size)
{
    do_asm("; ARRAY INDEX");
    // The current array index the framework needs us to resolve it at runtime.
    std::shared_ptr<Branch> child = array_index_branch->getValueBranch();
    // Save AX incase previously used
    do_asm("push ax");
    if (child->getType() == "E")
    {
        // This is an expression.
        make_expression(child, s_info);
    }
    else
    {
        make_move_reg_variable("ax", std::dynamic_pointer_cast<VarIdentifierBranch>(child), s_info);
    }
    // Ok now we need to multiply AX by the element size so that the offset points correctly
    do_asm("mov cx, " + std::to_string(elem_size));
    do_asm("mul cx");
    do_asm("mov di, ax");
    // Restore AX
    do_asm("pop ax");
}

int CodeGen8086::getSizeOfVariableBranch(std::shared_ptr<VDEFBranch> vdef_branch)
{
    return this->compiler->getSizeOfVarDef(vdef_branch);
}

int CodeGen8086::getFunctionArgumentIndex(std::shared_ptr<Branch> var_branch)
{
    std::string var_name;

    if (var_branch->getType() == "STRUCT_ACCESS")
    {
        std::shared_ptr<STRUCTAccessBranch> s_access_branch = std::dynamic_pointer_cast<STRUCTAccessBranch>(var_branch);
        var_branch = s_access_branch->getFirstChild();
    }

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

int CodeGen8086::getOffsetForGlobalVariable(std::shared_ptr<Branch> var_branch)
{
    return 0;
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
    std::shared_ptr<Branch> struct_def_name_branch = struct_def->getNameBranch();
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

std::shared_ptr<VDEFBranch> CodeGen8086::getVariableFromStructure(std::shared_ptr<STRUCTBranch> structure, std::string var_name)
{
    for (std::shared_ptr<Branch> branch : structure->getStructBodyBranch()->getChildren())
    {
        if (branch->getType() == "V_DEF" ||
                branch->getType() == "STRUCT_DEF")
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
            size += compiler->getPrimitiveDataTypeSize(data_type_branch->getValue());
        }
    }

    return size;
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
            size += getSizeOfVariableBranch(vdef_branch);
        }
    }

    return size;
}

int CodeGen8086::getBPOffsetForScopeVariable(std::shared_ptr<Branch> var_branch)
{
    std::shared_ptr<VDEFBranch> vdef_branch = getVariable(var_branch);
    return vdef_branch->getPositionRelZero(false);
}

bool CodeGen8086::hasGlobalVariable(std::shared_ptr<VarIdentifierBranch> var_branch)
{
    std::string var_name = var_branch->getVariableNameBranch()->getValue();
    for (std::shared_ptr<VDEFBranch> global_var_branch : this->global_variables)
    {
        std::string global_var_name = global_var_branch->
                getVariableIdentifierBranch()->getVariableNameBranch()->getValue();
        if (var_name == global_var_name)
            return true;
    }
    return false;
}

bool CodeGen8086::hasScopeVariable(std::shared_ptr<Branch> var_branch)
{
    std::string var_name;
    std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);

    var_name = var_iden_branch->getVariableNameBranch()->getValue();

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

std::string CodeGen8086::convert_full_reg_to_low_reg(std::string reg)
{
    if (reg == "ax")
    {
        reg = "al";
    }
    else if (reg == "bx")
    {
        reg = "bl";
    }
    else if (reg == "cx")
    {
        reg = "cl";
    }
    else if (reg == "dx")
    {
        reg = "dl";
    }

    return reg;
}

void CodeGen8086::scope_handle_func(struct stmt_info* s_info, struct position_info* pos_info)
{
    if (pos_info->is_root)
    {
        do_asm("mov bx, [bp-" + std::to_string(pos_info->abs_pos) + "]");
    }
}

struct VARIABLE_ADDRESS CodeGen8086::getASMAddressForVariable(struct stmt_info* s_info, std::shared_ptr<VarIdentifierBranch> root_var_branch, bool to_variable_start_only)
{
    struct VARIABLE_ADDRESS address;
    address.apply_reg = "";
    std::shared_ptr<VDEFBranch> top_vdef_branch = root_var_branch->getVariableDefinitionBranch(true);
    std::shared_ptr<VDEFBranch> vdef_branch = root_var_branch->getVariableDefinitionBranch();
    VARIABLE_TYPE var_type = vdef_branch->getVariableType();

    POSITION_OPTIONS options = 0;


    if (root_var_branch->isPositionStatic() || to_variable_start_only)
    {
        // The position is guaranteed to be static as we have checked for it so we won't need to pass a handle function.
        options |= POSITION_OPTION_POSITION_STATIC_IGNORE_HANDLE_FUNCTION;

        // We have a static position so this variable position can be calculated at compile time
        switch (var_type)
        {
        case VARIABLE_TYPE_GLOBAL_VARIABLE:
            address.segment = "_data";
            address.op = "+";
            address.offset = root_var_branch->getPositionRelZero(NULL, NULL);
            break;
        case VARIABLE_TYPE_FUNCTION_VARIABLE:
            if (!to_variable_start_only && (root_var_branch->hasStructureAccessBranch() || root_var_branch->hasRootArrayIndexBranch()))
            {
                // Function arrays must start at the end and work their way to the beginning as well as function structures.
                int position_to_root_elem = root_var_branch->getRootPositionRelZero(options | POSITION_OPTION_STOP_AT_ROOT_VAR);
                int elem_size = top_vdef_branch->getSize();
                int minus = position_to_root_elem + elem_size;
                address.op = "+";
                address.segment = "bp-" + std::to_string(minus);
                // This will get the position while ignoring the current scope, essentially it is the position relative to the structure or array.
                address.offset = root_var_branch->getPositionRelZeroIgnoreCurrentScope(NULL, NULL, options);
            }
            else
            {
                address.segment = "bp";
                address.op = "-";
                options |= POSITION_OPTION_START_WITH_VARSIZE;
                if (to_variable_start_only)
                {
                    options |= POSITION_OPTION_STOP_AT_ROOT_VAR;
                }
                address.offset = root_var_branch->getPositionRelZero(NULL, NULL, options);
            }
            break;

        case VARIABLE_TYPE_FUNCTION_ARGUMENT_VARIABLE:
            address.segment = "bp";
            address.op = "+";
            options = POSITION_OPTION_STOP_AT_ROOT_VAR;
            // + 4 due to return address and new base pointer
            address.offset = root_var_branch->getPositionRelZero(NULL, NULL, options) + 4;
            break;

        }
    }
    else
    {

        // Ok the position is non-static we will need to deal with it at run time
        address.segment = "bx";
        address.op = "+";
        options = POSITION_OPTION_START_WITH_VARSIZE;
        switch (var_type)
        {
        case VARIABLE_TYPE_GLOBAL_VARIABLE:
        {

        }
            break;
        case VARIABLE_TYPE_FUNCTION_VARIABLE:
        {
            address.offset = root_var_branch->getPositionRelZero([&](struct position_info * pos_info) -> void
                                                                 {
                                                                     scope_handle_func(s_info, pos_info);
                                                                 },
                                                                 [&](int rel_position) -> void
                                                                 {
                                                                     do_asm("mov bx, [bx+" + std::to_string(rel_position) + "]");
                                                                 }, options);
        }
            break;
        case VARIABLE_TYPE_FUNCTION_ARGUMENT_VARIABLE:
        {

        }
            break;

        }
    }
    /*
switch (var_type)
{
case VARIABLE_TYPE_GLOBAL_VARIABLE:
    address.segment = "data";
    address.op = "+";
    address.offset = var_branch->getPositionRelZero(array_unpredictable_func, struct_access_unpredictable_func);
    break;
case VARIABLE_TYPE_FUNCTION_ARGUMENT_VARIABLE:
    address.segment = "bp";
    address.op = "+";
    options = POSITION_OPTION_STOP_AT_ROOT_VAR;
    // + 4 due to return address and new base pointer
    address.offset = var_branch->getPositionRelZero(array_unpredictable_func, struct_access_unpredictable_func, options) + 4;
    break;
 
case VARIABLE_TYPE_FUNCTION_VARIABLE:
    if (!to_variable_start_only && (var_branch->hasStructureAccessBranch() || var_branch->hasRootArrayIndexBranch()))
    {
        // Function arrays must start at the end and work their way to the beginning as well as function structures.
        int position_to_root_elem = var_branch->getRootPositionRelZero(POSITION_OPTION_STOP_AT_ROOT_VAR);
        int elem_size = top_vdef_branch->getSize();
        int minus = position_to_root_elem + elem_size;
        address.op = "+";
        address.segment = "bp-" + std::to_string(minus);
        if (address.apply_reg != "")
        {
            address.segment += " " + address.apply_reg;
        }
        // This will get the position while ignoring the current scope, essentially it is the position relative to the structure or array.
        address.offset = var_branch->getPositionRelZeroIgnoreCurrentScope(abs_gen_func, array_unpredictable_func, struct_access_unpredictable_func);
    }
    else
    {
        address.segment = "bp";
        address.op = "-";
        options = POSITION_OPTION_START_WITH_VARSIZE;
        if (to_variable_start_only)
        {
            options |= POSITION_OPTION_STOP_AT_ROOT_VAR;
        }
        address.offset = var_branch->getPositionRelZero(abs_gen_func, array_unpredictable_func, struct_access_unpredictable_func, options);
    }
    break;
}
     */


    return address;

}

std::string CodeGen8086::getASMAddressForVariableFormatted(struct stmt_info* s_info, std::shared_ptr<VarIdentifierBranch> root_var_branch, bool to_variable_start_only)
{
    struct VARIABLE_ADDRESS address = getASMAddressForVariable(s_info, root_var_branch, to_variable_start_only);
    return address.to_string();
}

std::shared_ptr<Branch> CodeGen8086::getScopeVariable(std::shared_ptr<Branch> var_branch)
{
    /* If this is a structure access e.g "a.b.c.d" then the variable we need to return is "d"*/
    std::shared_ptr<STRUCTAccessBranch> access_branch = NULL;
    std::shared_ptr<Branch> var_name_branch = NULL;


    std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
    var_name_branch = var_iden_branch->getVariableNameBranch();


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
                        struct_branch = getStructure(struct_def->getDataTypeBranch()->getDataType());
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

std::shared_ptr<Branch> CodeGen8086::getGlobalVariable(std::shared_ptr<Branch> var_branch)
{
    std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
    std::shared_ptr<Branch> var_iden_name_branch = var_iden_branch->getVariableNameBranch();
    for (std::shared_ptr<Branch> global_var : this->global_variables)
    {
        std::shared_ptr<VDEFBranch> global_var_def_branch = std::dynamic_pointer_cast<VDEFBranch>(global_var);
        std::shared_ptr<Branch> global_var_def_name_branch = global_var_def_branch->getNameBranch();
        if (global_var_def_name_branch->getValue() == var_iden_name_branch->getValue())
        {
            return global_var_def_branch;
        }
    }

    return NULL;
}

std::shared_ptr<VDEFBranch> CodeGen8086::getVariable(std::shared_ptr<Branch> var_branch)
{
    std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
    return var_iden_branch->getVariableDefinitionBranch();
    /*
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
    case GLOBAL_VARIABLE:
        variable_branch = getGlobalVariable(var_branch);
        break;
    }

    return std::dynamic_pointer_cast<VDEFBranch>(variable_branch);
     */
}

std::shared_ptr<Branch> CodeGen8086::getFunctionArgumentVariable(std::shared_ptr<Branch> var_branch)
{
    std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
    std::string var_iden_name = var_iden_branch->getVariableNameBranch()->getValue();
    for (std::shared_ptr<Branch> variable : this->func_arguments)
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(variable);
        std::shared_ptr<Branch> variable_name_branch = vdef_branch->getNameBranch();
        if (variable_name_branch->getValue() == var_iden_name)
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

bool CodeGen8086::is_alone_var_to_be_word(std::shared_ptr<VDEFBranch> vdef_branch, bool ignore_pointer)
{
    return (!ignore_pointer && vdef_branch->getDataTypeBranch()->isPointer()) || vdef_branch->getDataTypeBranch()->getDataTypeSize(ignore_pointer) == 2;
}

bool CodeGen8086::is_alone_var_to_be_word(std::shared_ptr<VarIdentifierBranch> var_branch, bool ignore_pointer)
{
    if (!var_branch->isVariableAlone())
    {
        throw CodeGeneratorException("CodeGen8086::is_alone_var_to_be_word(std::shared_ptr<VarIdentifierBranch> var_branch): can only handle alone variables");
    }

    std::shared_ptr<VDEFBranch> vdef_branch = var_branch->getVariableDefinitionBranch();
    return is_alone_var_to_be_word(vdef_branch, ignore_pointer);
}

void CodeGen8086::generate_global_branch(std::shared_ptr<Branch> branch)
{
    // We don't really take advantage of this statement info here as this is not a statement, but we don't want to pass a NULL 
    struct stmt_info s_info;

    if (branch->getType() == "V_DEF" ||
            branch->getType() == "STRUCT_DEF")
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        handle_global_var_def(vdef_branch);
    }
    else if (branch->getType() == "FUNC")
    {
        std::shared_ptr<FuncBranch> func_branch = std::dynamic_pointer_cast<FuncBranch>(branch);
        handle_function(func_branch);
    }
    else if (branch->getType() == "FUNC_DEF")
    {
        std::shared_ptr<FuncDefBranch> func_def_branch = std::dynamic_pointer_cast<FuncDefBranch>(branch);
        handle_function_definition(func_def_branch);
    }
    else if (branch->getType() == "ASM")
    {
        std::shared_ptr<ASMBranch> asm_branch = std::dynamic_pointer_cast<ASMBranch>(branch);
        make_inline_asm(&s_info, asm_branch);
    }
    else if (branch->getType() == "STRUCT")
    {
        std::shared_ptr<STRUCTBranch> struct_branch = std::dynamic_pointer_cast<STRUCTBranch>(branch);
        handle_structure(struct_branch);
    }
}

void CodeGen8086::assemble(std::string assembly)
{
#ifdef DEBUG_MODE
    std::cout << "FINAL ASSEMBLY" << std::endl;
#endif
    std::cout << assembly << std::endl;
    Assembler8086 assembler(getCompiler(), getObjectFormat());
    assembler.setInput(assembly);
    assembler.run();
}
