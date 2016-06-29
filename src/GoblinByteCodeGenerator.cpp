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

GoblinByteCodeGenerator::GoblinByteCodeGenerator(Compiler* compiler) : CodeGenerator(compiler)
{
    this->saved_pos = 0;
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
    if (branch->getType() == "FUNC")
    {
        std::shared_ptr<FuncBranch> funcBranch = std::dynamic_pointer_cast<FuncBranch>(branch);
        std::vector<std::shared_ptr < Branch>> func_argument_branches = funcBranch->getFunctionArgumentBranches();
        std::shared_ptr<Branch> scopeBranch = funcBranch->getFunctionScopeBranches();
        this->startRegistrationProcess();
        // Generate the scope
        this->saved_pos = this->stream->getPosition();
        // Register the function arguments as a scope variable
        for (std::shared_ptr<Branch> branch : func_argument_branches)
        {
            this->registerScopeVariable(branch);
        }
        this->handleScope(scopeBranch);
        // Write some instructions just before the data that was written in handleScope, this will set the data pointer accordingly.
        int new_pos = this->stream->getPosition();
        this->stream->setPosition(this->saved_pos);
        this->stream->startLoggingOffset();
        this->stream->write8(SUBDP);
        this->stream->write32(this->getScopeVariablesSize());

        // Add the stream difference to the new_pos variable since we just did some writing
        new_pos += this->stream->getLoggedOffset();
        this->stream->stopLoggingOffset();

        // Now set the position to what it should be
        this->stream->setPosition(new_pos);

        // Finally readjust the DP
        this->stream->write8(ADDDP);
        this->stream->write32(this->getScopeVariablesSize());


        this->clearScopeVariables();
        this->registerMemoryLocation(funcBranch);
    }
}

void GoblinByteCodeGenerator::handleScope(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "V_DEF")
    {
        this->registerScopeVariable(branch);
    }
    else if (branch->getType() == "ASSIGN")
    {
        std::shared_ptr<AssignBranch> assign_branch = std::dynamic_pointer_cast<AssignBranch>(branch);
        std::string var_name = branch->getChildren()[0]->getChildren()[0]->getValue();
        std::shared_ptr<struct scope_variable> var = this->getScopeVariable(var_name);
        // Handle the expression
        std::vector<std::shared_ptr<Branch>> e_branches = this->getCompiler()->getASTAssistant()->findAllChildrenOfType(assign_branch->getValueBranch(), "MATH_E");
        for (std::shared_ptr<Branch> e_branch : e_branches)
        {
            std::shared_ptr<MathEBranch> math_e_branch = std::dynamic_pointer_cast<MathEBranch>(e_branch);
            handleExpression(math_e_branch);
        }
        // Now pop it from the stack and stick it into the correct memory location
        int mpos_rel = var->index * var->size;
        this->stream->write8(SPOP_TO_DP_RELATIVE);
        this->stream->write32(mpos_rel);
    }
    else if (branch->getType() == "CALL")
    {
        std::shared_ptr<CallBranch> callBranch = std::dynamic_pointer_cast<CallBranch>(branch);
        std::string name = callBranch->getFunctionNameBranch()->getValue();
        std::vector<std::shared_ptr < Branch>> arguments = callBranch->getFunctionArgumentBranches();
        if (this->isFunctionRegistered(name))
        {
            // This is a local function call
            int func_mem_loc = this->getFunctionMemoryLocation(name);
            for (std::shared_ptr<Branch> argument : arguments)
            {
                if (argument->getType() == "number")
                {
                    int number = std::stoi(argument->getValue(), NULL);
                    this->stream->write8(SPUSH);
                    this->stream->write32(number);
                }
            }
            this->stream->write8(CALL);
            this->stream->write32(func_mem_loc);
        }
        else
        {
            // This is a system function call
            throw CodeGeneratorException("System calls are currently not compatible");
        }
    }

    std::vector<std::shared_ptr < Branch>> children = branch->getChildren();
    for (std::shared_ptr<Branch> child : children)
    {
        handleScope(child);
    }
}

/* Handles only one part of an expression for example: 5 + 5 it would handle but 5 + 5 + 5 it would not. #
 * the method is only responsible for handling one part of an expression. */
void GoblinByteCodeGenerator::handleExpression(std::shared_ptr<Branch> branch)
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