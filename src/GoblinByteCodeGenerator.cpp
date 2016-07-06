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
 * File:   GoblinByteCodeGenerator.cpp
 * Author: Daniel McCarthy
 *
 * Created on 20 June 2016, 01:07
 * 
 * Description: The standard code generator for the Goblin compiler.
 * This code generator compiles directly into Goblin byte code.
 */

#include "GoblinByteCodeGenerator.h"
#include "CallBranch.h"
#include "FuncBranch.h"
#include "VDEFBranch.h"
#include "AssignBranch.h"
#include "MathEBranch.h"

GoblinByteCodeGenerator::GoblinByteCodeGenerator(Compiler* compiler, std::string code_gen_desc) : CodeGenerator(compiler, code_gen_desc)
{
    this->saved_pos = 0;
    this->linker = std::shared_ptr<Linker>(new GoblinByteCodeLinker(compiler));
}

GoblinByteCodeGenerator::~GoblinByteCodeGenerator()
{
}

void GoblinByteCodeGenerator::generate(std::shared_ptr<Tree> tree)
{
    CodeGenerator::generate(tree);
}

void GoblinByteCodeGenerator::generateFromBranch(std::shared_ptr<Branch> branch)
{
    CodeGenerator::generateFromBranch(branch);
}

std::shared_ptr<Linker> GoblinByteCodeGenerator::getLinker()
{
    return this->linker;
}

void GoblinByteCodeGenerator::scope_start(std::shared_ptr<Branch> branch)
{
    this->saved_pos = this->stream->getPosition();
    // This instruction will be replaced at the end of a scope.
    // It is required to keep the offsets correct for other calculations previously done.
    // See section 5 in the "FIXED_BUGS" file
    this->stream->write8(SUBDP);
    this->stream->write32(0);
}

void GoblinByteCodeGenerator::scope_assign_start(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> var)
{

}

void GoblinByteCodeGenerator::scope_assign_end(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> var)
{
    // Now pop it from the stack and stick it into the correct memory location
    int mpos_rel = var->index * var->size;
    this->stream->write8(SPOP_TO_DP_RELATIVE);
    this->stream->write32(mpos_rel);
}

void GoblinByteCodeGenerator::scope_end(std::shared_ptr<Branch> branch)
{
    this->stream->setPosition(this->saved_pos);
    this->stream->setEraseMode(true);
    this->stream->write8(SUBDP);
    this->stream->write32(this->getScopeVariablesSize());
    this->stream->setEraseMode(false);
    
    this->stream->setPosition(this->stream->getSize());
    // Finally readjust the DP
    this->stream->write8(ADDDP);
    this->stream->write32(this->getScopeVariablesSize());
}

void GoblinByteCodeGenerator::scope_func_call(std::shared_ptr<Branch> branch, std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments)
{
    this->stream->write8(CALL);
    // Placeholder address until the actual address is known during link time.
    this->stream->write32(0);
}

void GoblinByteCodeGenerator::scope_handle_exp(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "MATH_E")
    {
        std::shared_ptr<MathEBranch> m_branch = std::dynamic_pointer_cast<MathEBranch>(branch);
        std::shared_ptr<Branch> left = m_branch->getLeftBranch();
        std::shared_ptr<Branch> op = m_branch->getOperatorBranch();
        std::shared_ptr<Branch> right = m_branch->getRightBranch();

        bool left_ok = handleEntity(left);
        bool right_ok = handleEntity(right);

        if (left_ok || right_ok)
        {
            this->createStackMathInstruction(op->getValue());
        }
        else if (!left_ok && !right_ok)
        {
            this->createStackMathInstruction(op->getValue());
        }
    }
}

void GoblinByteCodeGenerator::scope_handle_number(std::shared_ptr<Branch> branch)
{
    int number = std::stoi(branch->getValue(), NULL);
    this->stream->write8(SPUSH);
    this->stream->write32(number);
}

bool GoblinByteCodeGenerator::handleEntity(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "number")
    {
        int number = std::stoi(branch->getValue(), NULL);
        this->stream->write8(SPUSH);
        this->stream->write32(number);
        std::cout << "SPUSH, " << std::stoi(branch->getValue(), NULL) << std::endl;
        return true;
    }
    else if (branch->getType() == "identifier")
    {
        // Get the address of the variable
        std::shared_ptr<struct scope_variable> var = this->getScopeVariable(branch->getValue());
        int mpos_rel = var->index * var->size;
        // Push the value of the memory pointed to by that address to the stack
        this->stream->write8(SPUSH_MEMORY_VALUE_DP_RELATIVE);
        this->stream->write32(mpos_rel);
        return true;
    }

    return false;
}

void GoblinByteCodeGenerator::createStackMathInstruction(std::string op)
{
    if (op == "+")
    {
        this->stream->write8(SADD);
        std::cout << "SADD" << std::endl;
    }
    else if (op == "-")
    {
        this->stream->write8(SSUB);
    }
    else if (op == "*")
    {
        this->stream->write8(SMUL);
        std::cout << "SMUL" << std::endl;
    }
    else if (op == "/")
    {
        this->stream->write8(SDIV);
    }
}