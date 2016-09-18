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
}

CodeGen8086::~CodeGen8086()
{
}

void CodeGen8086::make_label(std::string label)
{
    do_asm("_" + label + ":");
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
        make_assignment(name, value_exp);
    }
}

void CodeGen8086::make_assignment(std::string pos, std::shared_ptr<Branch> value_exp)
{
    // We have a value expression here so make it.
    make_expression(value_exp);
    // Now we must assign the variable with the expression result
    do_asm("mov [" + pos + "], ax");
}

void CodeGen8086::make_expression(std::shared_ptr<Branch> exp)
{
    // Only a number is provided this will be easy :)
    if (exp->getType() == "number")
    {
        do_asm("mov ax, " + exp->getValue());
    }
    else if (exp->getType() == "identifier")
    {
        // This is a variable so set AX to the value of this variable
        make_move_reg_variable("ax", exp->getValue());
    }
    else
    {
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
                if (left->getType() == "identifier")
                {
                    make_move_reg_variable("ax", left->getValue());
                }
                else
                {
                    // Its a number literal
                    do_asm("mov ax, " + left->getValue());
                }
            }
        }


        // Save the AX if we need to
        if (left->getType() == "E" && right->getType() == "E")
        {
            do_asm("push ax");
        }

        if (right->getType() == "E")
        {
            make_expression(right);
            if (left->getType() != "E")
            {
                do_asm("mov bx, " + left->getValue());
            }
        }
        else
        {
            if (right->getType() == "identifier")
            {
                make_move_reg_variable("bx", right->getValue());
            }
            else
            {
                // Its a number literal
                do_asm("mov bx, " + right->getValue());
            }
        }

        // Restore the AX if we need to
        if (left->getType() == "E" && right->getType() == "E")
        {
            do_asm("pop bx");
            // Now add the results together
            do_asm("add ax, bx");
        }
        else
        {
            make_math_instruction(exp->getValue(), "ax", "bx");
        }
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
    else
    {
        throw CodeGeneratorException("void CodeGen8086::make_math_instruction(std::string op): expecting a valid operator");
    }
}

void CodeGen8086::make_move_reg_variable(std::string reg_name, std::string var_name)
{
    int bp_offset;
    std::string bp_str;
    int var_type = this->getVariableType(var_name);
    switch (var_type)
    {
    case ARGUMENT_VARIABLE:
        bp_offset = getBPOffsetForArgument(var_name);
        bp_str = "[bp+" + std::to_string(bp_offset) + "]";
        break;
    case SCOPE_VARIABLE:
        bp_offset = getBPOffsetForScopeVariable(var_name);
        bp_str = "[bp-" + std::to_string(bp_offset) + "]";
        break;
    }

    do_asm("mov " + reg_name + ", " + bp_str);
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
    else if (branch->getType() == "V_DEF")
    {
        // Register a scope variable
        this->scope_variables.push_back(branch);
    }
}

void CodeGen8086::handle_scope_assignment(std::shared_ptr<AssignBranch> assign_branch)
{
    std::shared_ptr<Branch> var_to_assign_branch = assign_branch->getVariableToAssignBranch();
    std::shared_ptr<Branch> value = assign_branch->getValueBranch();

    int pos;
    std::string var_name = var_to_assign_branch->getValue();
    int var_type = this->getVariableType(var_name);
    switch (var_type)
    {
    case ARGUMENT_VARIABLE:
        pos = getBPOffsetForArgument(var_name);
        make_assignment("bp+" + std::to_string(pos), value);
        break;
    case SCOPE_VARIABLE:
        pos = getBPOffsetForScopeVariable(var_name);
        /* Naturally since "pos" will be minus when it is converted to a string the "-" symbol will be placed,
         *  so there is no need to define it*/
        make_assignment("bp-" + std::to_string(pos), value);
        break;
    }
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