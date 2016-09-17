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
    do_asm(label + ":");
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

void CodeGen8086::make_assignment(std::string var_name, std::shared_ptr<Branch> value_exp)
{
    // We have a value expression here so make it.
    make_expression(value_exp);
    // Now we must assign the variable with the expression result
    do_asm("mov [" + var_name + "], ax");
}

void CodeGen8086::make_expression(std::shared_ptr<Branch> exp)
{
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
            do_asm("mov ax, " + left->getValue());
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
        do_asm("mov bx, " + right->getValue());
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

void CodeGen8086::handle_global_var_def(std::shared_ptr<VDEFBranch> vdef_branch)
{
    std::string var_keyword_value = vdef_branch->getKeywordBranch()->getValue();
    std::string var_name_value = vdef_branch->getNameBranch()->getValue();
    make_variable(var_name_value, var_keyword_value, vdef_branch->getValueExpBranch());
}

void CodeGen8086::generate_global_branch(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "V_DEF")
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        handle_global_var_def(vdef_branch);
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