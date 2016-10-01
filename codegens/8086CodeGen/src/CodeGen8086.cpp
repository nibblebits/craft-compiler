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

CodeGen8086::CodeGen8086(Compiler* compiler) : CodeGenerator(compiler, "goblin_bytecode")
{
    this->linker = std::shared_ptr<Linker>(new GoblinByteCodeLinker(compiler));
    this->compiler = compiler;
    this->cmp_exp_false_label_name = "";
    this->cmp_exp_end_label_name = "";
    this->current_label_index = 0;
    this->is_cmp_expression = false;
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

void CodeGen8086::make_mem_assignment(std::string dest, std::shared_ptr<Branch> value_exp)
{
    // We have a value expression here so make it.
    make_expression(value_exp);

    // Handle any compare expression if any
    handle_compare_expression();

    // Now we must assign the variable with the expression result
    do_asm("mov [" + dest + "], ax");
}

void CodeGen8086::make_expression(std::shared_ptr<Branch> exp)
{
    if (exp->getType() == "number")
    {
        do_asm("mov ax, " + exp->getValue());
    }
    else if (exp->getType() == "identifier")
    {
        // This is a variable so set AX to the value of this variable
        make_move_reg_variable("ax", exp->getValue());
    }
    else if (exp->getType() == "PTR")
    {
        // This is pointer access to a variable so lets set AX to the value that it is pointing to
        handle_move_pointed_to_reg("ax", exp);
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
        std::shared_ptr<Branch> var_branch = address_of_branch->getVariableBranch();
        make_move_variable_address("ax", var_branch->getValue());
    }
    else
    {

        if (exp->getType() == "E")
        {
            std::string exp_val = exp->getValue();
            if (compiler->isLogicalOperator(exp_val))
            {
                this->cmp_exp_last_logic_operator = exp_val;
            }
        }

        // This is a proper expression so process it
        std::shared_ptr<Branch> left = exp->getFirstChild();
        std::shared_ptr<Branch> right = exp->getSecondChild();

        if (left->getType() == "E")
        {
            make_expression(left);
        }
        else
        {
            if (right->getType() != "E")
            {
                make_expression_left(left, "ax");
            }
        }


        // Save the AX if we need to
        if (left->getType() == "E" && right->getType() == "E")
        {
            //  do_asm("push ax");
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

            if (right->getType() == "identifier")
            {
                make_move_reg_variable("cx", right->getValue());
            }
            else if (right->getType() == "PTR")
            {
                handle_move_pointed_to_reg("cx", right);
            }
            else if (right->getType() == "FUNC_CALL")
            {
                /*
                 * This is a function call, we must push AX as at this point AX is set to something,
                 * the AX register is the register used to return data to the function caller.
                 * Therefore the previous AX register must be saved
                 */

                std::shared_ptr<FuncCallBranch> func_call_branch = std::dynamic_pointer_cast<FuncCallBranch>(right);

                // Save CX
                do_asm("push cx");
                handle_function_call(func_call_branch);
                // Since AX now contains returned value we must move it to register CX as this is where right operands get stored of any expression
                do_asm("mov bx, cx");
                // Restore CX
                do_asm("pop cx");
            }
            else
            {
                // Its a number literal
                do_asm("mov cx, " + right->getValue());
            }
        }

        // Restore the AX if we need to
        if (left->getType() == "E" && right->getType() == "E")
        {
            //   do_asm("pop bx");
            // Now add the results together
            //   do_asm("add ax, cx");
        }
        else
        {
            make_math_instruction(exp->getValue(), "ax", "cx");
        }
    }
}

void CodeGen8086::make_expression_left(std::shared_ptr<Branch> exp, std::string register_to_store)
{
    std::string type = exp->getType();
    std::string value = exp->getValue();

    if (type == "identifier")
    {
        make_move_reg_variable(register_to_store, value);
    }
    else if (type == "PTR")
    {
        handle_move_pointed_to_reg(register_to_store, exp);
    }
    else if (type == "FUNC_CALL")
    {
        // Its a function call
        std::shared_ptr<FuncCallBranch> func_call_branch = std::dynamic_pointer_cast<FuncCallBranch>(exp);
        handle_function_call(func_call_branch);
    }
    else
    {
        // Its a number literal
        do_asm("mov ax, " + value);
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
         * If the first register is ax then set the first register to the second register as "mul" instruction
         * uses ax as first reg regardless */
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
        // If we need to setup the compare labels then do it
        if (!this->is_cmp_expression)
        {
            this->cmp_exp_false_label_name = build_unique_label();
            this->cmp_exp_end_label_name = build_unique_label();
            this->cmp_exp_true_label = build_unique_label();
            is_cmp_expression = true;
        }

        // We must compare
        do_asm("cmp " + first_reg + ", " + second_reg);

        if (op == "==")
        {
            if (this->cmp_exp_last_logic_operator == "&&")
            {
                do_asm("jne " + this->cmp_exp_false_label_name);
            }
            else
            {
                // This is a logical else "||"
                do_asm("je " + this->cmp_exp_true_label);
            }
        }
        else if (op == "!=")
        {
            do_asm("je " + this->cmp_exp_false_label_name);
        }
        else if (op == "<=")
        {
            do_asm("jbe " + this->cmp_exp_false_label_name);
        }
        else if (op == ">=")
        {
            do_asm("jae " + this->cmp_exp_false_label_name);
        }
        else if (op == "<")
        {
            do_asm("jb " + this->cmp_exp_false_label_name);
        }
        else if (op == ">")
        {
            do_asm("ja " + this->cmp_exp_false_label_name);
        }
    }
    else
    {
        throw CodeGeneratorException("void CodeGen8086::make_math_instruction(std::string op): expecting a valid operator");
    }
}

void CodeGen8086::make_move_reg_variable(std::string reg, std::string var_name)
{
    std::string asm_addr = getASMAddressForVariable(var_name);
    do_asm("mov " + reg + ", [" + asm_addr + "]");
}

void CodeGen8086::make_move_variable_address(std::string reg_name, std::string var_name)
{
    int bp_offset;
    int var_type = this->getVariableType(var_name);

    if (var_type == ARGUMENT_VARIABLE || var_type == SCOPE_VARIABLE)
    {
        do_asm("mov " + reg_name + ", bp");
    }

    switch (var_type)
    {
    case ARGUMENT_VARIABLE:
        bp_offset = getBPOffsetForArgument(var_name);
        do_asm("add " + reg_name + ", " + std::to_string(bp_offset));
        break;
    case SCOPE_VARIABLE:
        bp_offset = getBPOffsetForScopeVariable(var_name);
        do_asm("sub " + reg_name + ", " + std::to_string(bp_offset));
        break;
    }
}

void CodeGen8086::make_var_assignment(std::string var_name, std::shared_ptr<Branch> value, bool pointer_assignment)
{
    // Check to see if we are assigning memory pointed to by a pointer
    std::string asm_addr = getASMAddressForVariable(var_name);

    // Are we accessing memory pointed to by a pointer?
    if (pointer_assignment)
    {
        do_asm("mov bx, [" + asm_addr + "]");
        // Set the memory pointed to by BX to value
        make_mem_assignment("bx", value);
    }
    else
    {
        make_mem_assignment(asm_addr, value);
    }
}

void CodeGen8086::handle_global_var_def(std::shared_ptr<VDEFBranch> vdef_branch)
{
    std::string var_keyword_value = vdef_branch->getKeywordBranch()->getValue();
    std::string var_name_value = vdef_branch->getNameBranch()->getValue();
    make_variable(var_name_value, var_keyword_value, vdef_branch->getValueExpBranch());
}

void CodeGen8086::handle_function(std::shared_ptr<FuncBranch> func_branch)
{
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
    handle_func_body(body_branch);

}

void CodeGen8086::handle_func_args(std::shared_ptr<Branch> arguments)
{
    for (std::shared_ptr<Branch> arg : arguments->getChildren())
    {
        this->func_arguments.push_back(arg);
    }
}

void CodeGen8086::handle_func_body(std::shared_ptr<Branch> body)
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
    else if (branch->getType() == "RETURN")
    {
        handle_scope_return(branch);
    }
    else if (branch->getType() == "V_DEF" || branch->getType() == "V_DEF_PTR")
    {
        handle_scope_variable_declaration(branch);
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

    /* Add to the stack pointer to avoid all those parameters we just pushed.
     * On the 8086 architecture each value pushed is one word in size, therefore its the total children * 2
     * this will change when structures are added to the equation. */

    do_asm("add sp, " + std::to_string((params.size() * 2)));
}

void CodeGen8086::handle_scope_assignment(std::shared_ptr<AssignBranch> assign_branch)
{
    std::shared_ptr<Branch> var_to_assign_branch = assign_branch->getVariableToAssignBranch();
    std::shared_ptr<Branch> value = assign_branch->getValueBranch();

    make_var_assignment(var_to_assign_branch->getValue(), value);
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
    std::shared_ptr<Branch> variable_name_branch = branch->getFirstChild();
    std::string asm_addr = getASMAddressForVariable(variable_name_branch->getValue());
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
    if (this->is_cmp_expression)
    {
        /* Do a jmp to the end here. This is required as the following expression: 10 == 10 || 13 == 13 && 12 == 12
          will cause the system to roll onto the false label should all be true*/

        do_asm("jmp " + this->cmp_exp_end_label_name);

        // Generate false label
        make_exact_label(this->cmp_exp_false_label_name);

        // Move zero to AX as this is false
        do_asm("mov ax, 0");
        // Jump to the end so it does not change to true
        do_asm("jmp " + this->cmp_exp_end_label_name);

        // Generate true label
        make_exact_label(this->cmp_exp_true_label);
        // Move one to the AX as this is true
        do_asm("mov ax, 1");
        // No need to generate any "jmp" instruction as naturally the code will run to the end

        // Generate end label
        make_exact_label(this->cmp_exp_end_label_name);

        // Now reset the compare expression
        this->is_cmp_expression = false;
    }
}

void CodeGen8086::handle_scope_variable_declaration(std::shared_ptr<Branch> branch)
{
    // Register a scope variable
    this->scope_variables.push_back(branch);

    // Handle the variable declaration
    std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
    std::string var_name = vdef_branch->getNameBranch()->getValue();
    std::shared_ptr<Branch> vdef_value = vdef_branch->getValueExpBranch();

    // Are we assigning it to anything?
    if (vdef_value != NULL)
    {
        make_var_assignment(var_name, vdef_value);
    }
}

int CodeGen8086::getFunctionArgumentIndex(std::string arg_name)
{
    for (int i = 0; i < this->func_arguments.size(); i++)
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(this->func_arguments.at(i));
        std::string vdef_var_name = vdef_branch->getNameBranch()->getValue();
        if (vdef_var_name == arg_name)
        {
            return i;
        }
    }

    return -1;
}

int CodeGen8086::getBPOffsetForArgument(std::string arg_name)
{
    // * 2 As stack elements are 16 bits wide, +4 because this is where first element begins
    return (getFunctionArgumentIndex(arg_name) * 2) + 4;
}

int CodeGen8086::getScopeVariableIndex(std::string arg_name)
{
    for (int i = 0; i < this->scope_variables.size(); i++)
    {
        std::shared_ptr<Branch> scope_var = this->scope_variables.at(i);
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(scope_var);
        std::shared_ptr<Branch> name_branch = vdef_branch->getNameBranch();
        if (name_branch->getValue() == arg_name)
        {
            return i;
        }
    }

    return -1;
}

int CodeGen8086::getBPOffsetForScopeVariable(std::string arg_name)
{
    return (this->getScopeVariableIndex(arg_name) * 2) + 2;
}

int CodeGen8086::getVariableType(std::string arg_name)
{
    if (this->getScopeVariableIndex(arg_name) != -1)
        return SCOPE_VARIABLE;
    if (this->getFunctionArgumentIndex(arg_name) != -1)
        return ARGUMENT_VARIABLE;

    return GLOBAL_VARIABLE;
}

std::string CodeGen8086::getASMAddressForVariable(std::string var_name)
{
    int var_type = this->getVariableType(var_name);
    int bp_offset;
    std::string dst_string;
    switch (var_type)
    {
    case ARGUMENT_VARIABLE:
        bp_offset = getBPOffsetForArgument(var_name);
        dst_string = "bp+" + std::to_string(bp_offset) + "";
        break;
    case SCOPE_VARIABLE:
        bp_offset = getBPOffsetForScopeVariable(var_name);
        dst_string = "bp-" + std::to_string(bp_offset) + "";
        break;
    }

    return dst_string;

}

std::shared_ptr<Branch> CodeGen8086::getScopeVariable(std::string var_name)
{
    for (std::shared_ptr<Branch> variable : this->scope_variables)
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(variable);
        std::shared_ptr<Branch> variable_name_branch = vdef_branch->getNameBranch();
        if (variable_name_branch->getValue() == var_name)
        {
            return variable;
        }
    }

    return NULL;
}

std::shared_ptr<Branch> CodeGen8086::getFunctionArgumentVariable(std::string arg_name)
{
    for (std::shared_ptr<Branch> variable : this->func_arguments)
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(variable);
        std::shared_ptr<Branch> variable_name_branch = vdef_branch->getNameBranch();
        if (variable_name_branch->getValue() == arg_name)
        {
            return variable;
        }
    }

    return NULL;
}

bool CodeGen8086::isVariablePointer(std::string var_name)
{
    std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(getScopeVariable(var_name));
    if (vdef_branch == NULL)
    {
        vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(getFunctionArgumentVariable(var_name));
    }

    if (vdef_branch != NULL)
    {
        std::shared_ptr<Branch> name_branch = vdef_branch->getNameBranch();
        if (name_branch->getValue() == var_name
                && vdef_branch->getType() == "V_DEF_PTR")
        {
            return true;
        }
    }
    return false;
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
}

void CodeGen8086::assemble(std::string assembly)
{
    std::cout << assembly << std::endl;
}

std::shared_ptr<Linker> CodeGen8086::getLinker()
{
    return this->linker;
}