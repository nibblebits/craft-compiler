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

CodeGen8086::CodeGen8086(Compiler* compiler) : CodeGenerator(compiler, "8086 CodeGenerator", POINTER_SIZE)
{
    this->linker = std::shared_ptr<Linker>(new GoblinByteCodeLinker(compiler));
    this->compiler = compiler;
    this->current_label_index = 0;
    this->is_cmp_expression = false;
    this->do_signed = false;
    this->is_handling_pointer = false;
    this->scope_size = 0;

    this->pointer_selected_variable = NULL;
    this->pointer_selected_var_iden = NULL;
    this->last_found_var_access_variable = NULL;


}

CodeGen8086::~CodeGen8086()
{
}

struct formatted_segment CodeGen8086::format_segment(std::string segment_name)
{
    struct formatted_segment segment;

    // Temp replacement due to the fact I can't change segment locations in the emulator, should change back once no longer emulating.
    segment.start_segment = segment_name + ":";
    segment.end_segment = "; END SEGMENT";
    // segment.start_segment = segment_name + " segment";
    //segment.end_segment = "ends";
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
    return "_" + label_name;
}

void CodeGen8086::setup_comparing()
{
    this->cmp_exp_false_label_name = build_unique_label();
    this->cmp_exp_end_label_name = build_unique_label();
    this->cmp_exp_true_label_name = build_unique_label();
    is_cmp_expression = true;
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

void CodeGen8086::make_inline_asm(std::shared_ptr<ASMBranch> asm_branch)
{
    std::string asm_str = asm_branch->getInstructionStartStringBranch()->getValue();
    for (std::shared_ptr<Branch> child_branch : asm_branch->getInstructionArgumentsBranch()->getChildren())
    {
        std::shared_ptr<ASMArgBranch> arg_child_branch = std::dynamic_pointer_cast<ASMArgBranch>(child_branch);
        std::shared_ptr<VarIdentifierBranch> arg_child_value_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(arg_child_branch->getArgumentValueBranch());
        // Assumed to be variable, will crash in certain cases.
        std::string asm_addr = getASMAddressForVariableFormatted(arg_child_value_branch);
        asm_str += asm_addr + arg_child_branch->getNextStringBranch()->getValue();
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

void CodeGen8086::handle_logical_expression(std::shared_ptr<Branch> exp_branch, bool should_setup)
{
    std::shared_ptr<Branch> left = exp_branch->getFirstChild();
    std::shared_ptr<Branch> right = exp_branch->getSecondChild();

    if (should_setup)
        setup_comparing();


    if (left->getType() != "E")
    {
        make_expression(left);
        // We must setup comparing as it would not have been don e for this variable.
        make_compare_instruction(">", "ax", "0");
        // If we have an logical OR then we must end the compare expression here
        if (exp_branch->getValue() == "||")
        {
            handle_compare_expression();
        }
    }
    else if (compiler->isLogicalOperator(left->getValue()))
    {
        handle_logical_expression(left, false);
    }
    else
    {
        make_expression(left);
    }

    if (right->getType() != "E")
    {
        make_expression(right);
        make_compare_instruction(">", "ax", "0");
    }
    else if (compiler->isLogicalOperator(right->getValue()))
    {
        handle_logical_expression(right, false);
    }
    else
    {
        make_expression(right);
    }

    handle_compare_expression();
}

void CodeGen8086::make_expression(std::shared_ptr<Branch> exp, std::function<void() > exp_start_func, std::function<void() > exp_end_func, bool postpone_pointer)
{

    std::shared_ptr<Branch> left = NULL;
    std::shared_ptr<Branch> right = NULL;

    // Only if we are currently handling a pointer should we have to postpone it.
    if (postpone_pointer)
    {
        // Postpone any pointer handling going on as its unrelated.
        postpone_pointer_handling();
    }

    // Do we have something we need to notify about starting this expression?
    if (exp_start_func != NULL)
    {
        exp_start_func();
    }

    /* In cases where this happens "a && b" "a" and "b" have no operators of there own
 so we need to make the compare instruction ourself, we will check if the variable is above zero.*/
    if (compiler->isLogicalOperator(exp->getValue()))
    {
        handle_logical_expression(exp);
    }
    else
    {
        if (exp->getType() != "E")
        {
            make_expression_left(exp, "ax");
        }
        else
        {
            left = exp->getFirstChild();
            right = exp->getSecondChild();

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

            if (left->getType() == "E")
            {
                make_expression(left);
            }
            else if (right->getType() != "E")
            {
                make_expression_left(left, "ax");
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

            // Restore the AX register if we need to
            if (left->getType() == "E" &&
                    right->getType() == "E")
            {
                do_asm("pop cx");
            }

            if (this->do_signed)
            {
                make_math_instruction(exp->getValue(), "al", "cl");
            }
            else
            {
                make_math_instruction(exp->getValue(), "ax", "cx");
            }

        }


        // Prepone any pointer handling going to restore the previous state.
        if (postpone_pointer)
            prepone_pointer_handling();

        // Do we have something we need to notify about ending this expression?
        if (exp_end_func != NULL)
        {
            exp_end_func();
        }
    }
}

void CodeGen8086::make_expression_part(std::shared_ptr<Branch> exp, std::string register_to_store)
{
    if (exp->getType() == "number")
    {
        do_asm("mov " + register_to_store + ", " + exp->getValue());
    }
    else if (exp->getType() == "string")
    {
        std::string addr_to_str = make_string(exp);
        do_asm("lea " + register_to_store + ", " + addr_to_str);
    }
    else if (exp->getType() == "VAR_IDENTIFIER")
    {
        if (this->is_handling_pointer)
        {
            // When handling pointers only addresses should be calculated the value should not be brought back
            std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(exp);
            if (this->pointer_selected_variable == NULL)
            {
                // No variable yet set, set it to us
                this->pointer_selected_variable = var_iden_branch->getVariableDefinitionBranch();
                this->pointer_selected_var_iden = var_iden_branch;
            }
            std::string pos = make_var_access(var_iden_branch);
            do_asm("mov " + register_to_store + ", [" + pos + "]");
        }
        else
        {
            // This is a variable so set register to store to the value of this variable
            make_move_reg_variable(register_to_store, std::dynamic_pointer_cast<VarIdentifierBranch>(exp));
        }
    }
    else if (exp->getType() == "LOGICAL_NOT")
    {
        make_logical_not(std::dynamic_pointer_cast<LogicalNotBranch>(exp), register_to_store);
    }
    else if (exp->getType() == "PTR")
    {
        std::shared_ptr<PTRBranch> ptr_branch = std::dynamic_pointer_cast<PTRBranch>(exp);
        handle_ptr(ptr_branch);

        /* If the selected pointer variable is primitive then we should assign AX or AL to the value 
         * the pointer is pointing to, if its not primitive then we should not do anything as it must be
         a structure. */
        if (this->pointer_selected_variable != NULL)
        {
            if (this->pointer_selected_variable->isPrimitive())
            {
                if (this->pointer_selected_variable->getDataTypeSize(true) == 2)
                {
                    // This is pointing to a word.
                    do_asm("mov " + register_to_store + ", [bx]");
                }
                else
                {

                    // This is pointing to a byte 
                    do_asm("xor " + register_to_store + ", " + register_to_store);
                    do_asm("mov " + convert_full_reg_to_low_reg(register_to_store) + ", [bx]");
                }
            }
        }

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
}

void CodeGen8086::make_expression_left(std::shared_ptr<Branch> exp, std::string register_to_store)
{
    /* Blank AH ready for 8 bit identifiers.
     * This is required due to data left in the AH register due to previous expressions,
     * I believe a better alternative can be thought of but for now this will do.
     */
    if (exp->getType() == "VAR_IDENTIFIER")
    {
        std::shared_ptr<VDEFBranch> vdef_branch = getVariable(exp);
        if (!vdef_branch->isPointer() && vdef_branch->getDataTypeSize() == 1)
        {
            do_asm("xor ah, ah");
        }
    }
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
        /* Blank CH ready for 8 bit identifiers.
         * This is required due to data left in the CH register due to previous expressions,
         * I believe a better alternative can be thought of but for now this will do.
         */
        if (exp->getType() == "VAR_IDENTIFIER")
        {
            std::shared_ptr<VDEFBranch> vdef_branch = getVariable(exp);
            if (vdef_branch->getDataTypeSize() == 1)
            {
                do_asm("xor ch, ch");
            }
        }

        make_expression_part(exp, "cx");
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

void CodeGen8086::make_move_reg_variable(std::string reg, std::shared_ptr<VarIdentifierBranch> var_branch)
{
    std::shared_ptr<VDEFBranch> vdef_branch = var_branch->getVariableDefinitionBranch();
    std::string pos = make_var_access(var_branch);
    if (vdef_branch->isSigned())
    {
        this->do_signed = true;
    }

    do_asm("; MAKE MOVE REG VARIABLE");
    if (!vdef_branch->isPointer() && vdef_branch->getDataTypeSize() == 1)
    {
        // We don't want anything left in the register so lets blank it
        do_asm("xor " + reg + ", " + reg);

        /* Bytes must use the lower end of the registers and not the full register
         * we must break it down here before continuing */
        reg = convert_full_reg_to_low_reg(reg);
    }

    do_asm("mov " + reg + ", " + "[" + pos + "]");

}

void CodeGen8086::make_move_var_addr_to_reg(std::string reg_name, std::shared_ptr<VarIdentifierBranch> var_branch)
{
    std::string pos = make_var_access(var_branch);
    do_asm("lea " + reg_name + ", [" + pos + "]");
}

void CodeGen8086::make_array_offset_instructions(std::shared_ptr<ArrayIndexBranch> array_branch, int size_p_elem)
{
    std::shared_ptr<ArrayIndexBranch> current_branch = array_branch;

    int c = 0;
    while (true)
    {
        std::shared_ptr<Branch> value_branch = current_branch->getValueBranch();
        // Make the expression of the array index
        make_expression(value_branch);

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

void CodeGen8086::make_var_access_rel_base(std::shared_ptr<VarIdentifierBranch> var_branch, std::shared_ptr<VDEFBranch>* vdef_in_question_branch, std::shared_ptr<VarIdentifierBranch>* var_access_iden_branch, std::string base_reg, std::shared_ptr<STRUCTBranch> current_struct)
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
        make_array_offset_instructions(root_iden_branch->getRootArrayIndexBranch(), root_vdef_branch->getDataTypeSize(true));
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
                                 std::dynamic_pointer_cast<VarIdentifierBranch>(root_iden_branch->getStructureAccessBranch()->getFirstChild()),
                                 vdef_in_question_branch,
                                 var_access_iden_branch,
                                 base_reg,
                                 structure
                                 );
    }

}

std::string CodeGen8086::make_var_access(std::shared_ptr<VarIdentifierBranch> var_branch)
{
    std::string var_addr_formatted = getASMAddressForVariableFormatted(var_branch);

    return var_addr_formatted;
}

void CodeGen8086::make_var_assignment(std::shared_ptr<Branch> var_branch, std::shared_ptr<Branch> value)
{
    bool is_word;

    if (var_branch->getType() == "PTR")
    {
        std::shared_ptr<PTRBranch> ptr_branch = std::dynamic_pointer_cast<PTRBranch>(var_branch);
        handle_ptr(ptr_branch);
        is_word = is_alone_var_to_be_word(this->pointer_selected_variable, true);
        make_mem_assignment("bx", value, is_word);
    }
    else
    {
        std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(var_branch);
        std::shared_ptr<VDEFBranch> vdef_in_question_branch = var_iden_branch->getVariableDefinitionBranch();

        // Make the value expression
        make_expression(value);

        std::string pos = make_var_access(var_iden_branch);
        // This is a primitive type assignment, including pointer assignments
        is_word = is_alone_var_to_be_word(vdef_in_question_branch);
        make_mem_assignment(pos, NULL, is_word);

    }
}

void CodeGen8086::make_logical_not(std::shared_ptr<LogicalNotBranch> logical_not_branch, std::string register_to_store)
{
    make_expression(logical_not_branch->getSubjectBranch());
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

    // Generate some ASM to reserve space on the stack for this scope
    do_asm("sub sp, " + std::to_string(this->scope_size));

    current_scopes.push_back(this->scope_size);

}

void CodeGen8086::reset_scope_size()
{
    // Add the stack pointer by the scope size so the memory is recycled.
    do_asm("add sp, " + std::to_string(this->scope_size));
    current_scopes.pop_back();

    if (current_scopes.empty())
    {
        this->scope_size = 0;
    }
    else
    {
        this->scope_size = current_scopes.back();
    }
}

void CodeGen8086::handle_ptr(std::shared_ptr<PTRBranch> ptr_branch)
{
    this->pointer_selected_variable = NULL;
    this->pointer_selected_var_iden = NULL;
    this->is_handling_pointer = true;

    do_asm("; POINTER HANDLING ");

    std::shared_ptr<Branch> exp_branch = ptr_branch->getExpressionBranch();
    // False to prevent postponing of the pointer
    make_expression(exp_branch, NULL, NULL, false);
    // Memory address in question is stored in AX but we need it in BX so it can be accessed later.
    do_asm("mov bx, ax");

    /* If the pointer has structure access or array access then we must calculate it as if no variables exist on the scope 
     * then append the offset to the BX register. */

    if (pointer_selected_var_iden != NULL)
    {
        if (pointer_selected_var_iden->hasStructureAccessBranch() ||
                pointer_selected_var_iden->hasRootArrayIndexBranch())
        {
            int pos = pointer_selected_var_iden->getPositionRelZeroIgnoreCurrentScope([&](std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size)
            {
                handle_array_index(array_index_branch, elem_size);
                do_asm("add bx, di");
            });

            do_asm("add bx, " + std::to_string(pos));
        }
    }

    do_asm("; END OF POINTER HANDLING ");

    this->is_handling_pointer = false;
}

void CodeGen8086::handle_global_var_def(std::shared_ptr<VDEFBranch> vdef_branch)
{
    // Push this variable definition to the global variables vector
    this->global_variables.push_back(vdef_branch);

    // Handle the variable declaration
    std::shared_ptr<VarIdentifierBranch> variable_iden_branch = vdef_branch->getVariableIdentifierBranch();
    std::shared_ptr<Branch> variable_data_type_branch = vdef_branch->getDataTypeBranch();
    std::shared_ptr<Branch> variable_name_branch = variable_iden_branch->getVariableNameBranch();
    std::shared_ptr<Branch> value_branch = vdef_branch->getValueExpBranch();

    make_label(variable_name_branch->getValue(), "data");

    // NOTE: STRUCT_DEF'S CAN ALSO BE POINTERS!!!! CHANGE THIS 
    if (vdef_branch->getType() == "STRUCT_DEF")
    {
        int struct_size = getSizeOfVariableBranch(vdef_branch);
        do_asm("rb " + std::to_string(struct_size), "data");
    }
    else
    {
        if (vdef_branch->isPointer() || vdef_branch->getDataTypeSize() == 2)
        {
            do_asm("dw 0", "data");
        }
        else
        {
            do_asm("db 0", "data");
        }
    }

}

void CodeGen8086::handle_structure(std::shared_ptr<STRUCTBranch> struct_branch)
{
    this->structures.push_back(struct_branch);
}

void CodeGen8086::handle_function(std::shared_ptr<FuncBranch> func_branch)
{
    // Clear previous scope variables from other functions
    this->scope_variables.clear();

    std::shared_ptr<Branch> name_branch = func_branch->getNameBranch();
    std::shared_ptr<Branch> arguments_branch = func_branch->getArgumentsBranch();
    std::shared_ptr<Branch> body_branch = func_branch->getBodyBranch();

    // Make the function label
    make_label(name_branch->getValue());

    do_asm("push bp");
    do_asm("mov bp, sp");

    // Handle the arguments
    handle_func_args(arguments_branch);

    // First we need to calculate the entire scope size, its important
    calculate_scope_size(std::dynamic_pointer_cast<ScopeBranch>(body_branch));

    // Handle the body
    handle_body(body_branch);


}

void CodeGen8086::handle_func_args(std::shared_ptr<Branch> arguments)
{
    this->func_arguments.clear();
    for (std::shared_ptr<Branch> arg : arguments->getChildren())
    {
        std::shared_ptr<VDEFBranch> arg_vdef = std::dynamic_pointer_cast<VDEFBranch>(arg);
        // All function arguments are 2 bytes in size.
        arg_vdef->setCustomDataTypeSize(2);
        this->func_arguments.push_back(arg_vdef);
    }
}

void CodeGen8086::handle_body(std::shared_ptr<Branch> body)
{
    // Now we can handle every statement
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
    else if (branch->getType() == "ASM")
    {
        std::shared_ptr<ASMBranch> asm_branch = std::dynamic_pointer_cast<ASMBranch>(branch);
        make_inline_asm(asm_branch);
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
}

void CodeGen8086::handle_function_call(std::shared_ptr<FuncCallBranch> branch)
{
    std::shared_ptr<Branch> func_name_branch = branch->getFuncNameBranch();
    std::shared_ptr<Branch> func_params_branch = branch->getFuncParamsBranch();

    std::vector<std::shared_ptr < Branch>> params = func_params_branch->getChildren();

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
    do_asm("add sp, " + std::to_string(params.size() * 2));
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

    reset_scope_size();

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
    std::shared_ptr<BODYBranch> body_branch = branch->getBodyBranch();

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

    calculate_scope_size(body_branch);

    // Handle the "IF" statements body.
    handle_body(body_branch);

    reset_scope_size();

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

    // Calculate the scope size for the "for" loop
    calculate_scope_size(std::dynamic_pointer_cast<ScopeBranch>(branch));

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

    reset_scope_size();

}

void CodeGen8086::handle_array_index(std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size)
{
    // The current array index the framework needs us to resolve it at runtime.
    std::shared_ptr<Branch> child = array_index_branch->getValueBranch();
    // Save AX incase previously used
    do_asm("push ax");
    if (child->getType() == "E")
    {
        // This is an expression.
        make_expression(child);
    }
    else
    {
        make_move_reg_variable("ax", std::dynamic_pointer_cast<VarIdentifierBranch>(child));
    }
    do_asm("mov cx, " + std::to_string(elem_size));
    do_asm("mul cx");
    do_asm("mov di, ax");
    // Restore AX
    do_asm("pop ax");
}

bool CodeGen8086::has_postponed_pointer_handling()
{
    return !this->current_pointers_to_handle.empty();
}

void CodeGen8086::postpone_pointer_handling()
{
    struct HANDLING_POINTER handling_ptr;
    handling_ptr.is_handling = this->is_handling_pointer;
    handling_ptr.pointer_selected_variable = this->pointer_selected_variable;
    handling_ptr.pointer_selected_var_iden = this->pointer_selected_var_iden;
    this->current_pointers_to_handle.push_back(handling_ptr);

    // Now reset
    this->is_handling_pointer = false;
    this->pointer_selected_variable = NULL;
    this->pointer_selected_var_iden = NULL;
}

void CodeGen8086::prepone_pointer_handling()
{
    struct HANDLING_POINTER handling_ptr = this->current_pointers_to_handle.back();
    this->is_handling_pointer = handling_ptr.is_handling;
    this->pointer_selected_variable = handling_ptr.pointer_selected_variable;
    this->pointer_selected_var_iden = handling_ptr.pointer_selected_var_iden;
    this->current_pointers_to_handle.pop_back();
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

struct VARIABLE_ADDRESS CodeGen8086::getASMAddressForVariable(std::shared_ptr<VarIdentifierBranch> var_branch)
{
    struct VARIABLE_ADDRESS address;
    address.apply_reg = "";
    std::shared_ptr<VDEFBranch> top_vdef_branch = var_branch->getVariableDefinitionBranch(true);
    std::shared_ptr<VDEFBranch> vdef_branch = var_branch->getVariableDefinitionBranch();
    VARIABLE_TYPE var_type = vdef_branch->getVariableType();

    std::function<void(std::shared_ptr<ArrayIndexBranch>, int) > unpredictable_func = [&](std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size)
    {
        handle_array_index(array_index_branch, elem_size);
        address.apply_reg = "di";
    };
    switch (var_type)
    {
    case VARIABLE_TYPE_GLOBAL_VARIABLE:
        address.segment = "data";
        address.op = "+";
        address.offset = var_branch->getPositionRelZero(unpredictable_func);
        break;
    case VARIABLE_TYPE_FUNCTION_ARGUMENT_VARIABLE:
        address.segment = "bp";
        address.op = "+";
        // + 4 due to return address and new base pointer
        address.offset = var_branch->getPositionRelZero(unpredictable_func) + 4;
        break;
    case VARIABLE_TYPE_FUNCTION_VARIABLE:
        if (!top_vdef_branch->isPointer() &&
                (var_branch->hasStructureAccessBranch() || var_branch->hasRootArrayIndexBranch()))
        {
            // Function arrays must start at the end and work their way to the beginning as well as function structures.
            address.op = "+";
            address.segment = "bp-" + std::to_string(top_vdef_branch->getSize());
            address.offset = var_branch->getPositionRelZero(unpredictable_func);
        }
        else
        {
            address.segment = "bp";
            address.op = "-";
            address.offset = var_branch->getPositionRelZero(unpredictable_func, true);
        }
        break;
    }

    return address;

}

std::string CodeGen8086::getASMAddressForVariableFormatted(std::shared_ptr<VarIdentifierBranch> var_branch)
{
    struct VARIABLE_ADDRESS address = getASMAddressForVariable(var_branch);
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
    return (!ignore_pointer && vdef_branch->isPointer()) || vdef_branch->getDataTypeSize(ignore_pointer) == 2;
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
    else if (branch->getType() == "ASM")
    {
        std::shared_ptr<ASMBranch> asm_branch = std::dynamic_pointer_cast<ASMBranch>(branch);
        make_inline_asm(asm_branch);
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