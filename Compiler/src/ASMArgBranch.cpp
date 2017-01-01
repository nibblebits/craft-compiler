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
 * File:   ASMArgBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 01 December 2016, 10:09
 * 
 * Description: 
 */

#include "ASMArgBranch.h"
#include "CustomBranch.h"

ASMArgBranch::ASMArgBranch(Compiler* compiler) : CustomBranch(compiler, "ASM_ARG", "")
{
}

ASMArgBranch::~ASMArgBranch()
{
}

void ASMArgBranch::setArgumentValueBranch(std::shared_ptr<Branch> arg_value_branch)
{
    CustomBranch::registerBranch("argument_value_branch", arg_value_branch);
}

void ASMArgBranch::setNextStringBranch(std::shared_ptr<Branch> next_string_branch)
{
    CustomBranch::registerBranch("next_string_branch", next_string_branch);
}

std::shared_ptr<Branch> ASMArgBranch::getArgumentValueBranch()
{
    return CustomBranch::getRegisteredBranchByName("argument_value_branch");
}

std::shared_ptr<Branch> ASMArgBranch::getNextStringBranch()
{
    return CustomBranch::getRegisteredBranchByName("next_string_branch");
}

void ASMArgBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<ASMArgBranch> asm_cloned_arg_branch = std::dynamic_pointer_cast<ASMArgBranch>(cloned_branch);
    std::shared_ptr<Branch> argument_value_branch = getArgumentValueBranch()->clone();
    std::shared_ptr<Branch> next_string_branch = getNextStringBranch()->clone();
    asm_cloned_arg_branch->setArgumentValueBranch(argument_value_branch);
    asm_cloned_arg_branch->setNextStringBranch(next_string_branch);
}

std::shared_ptr<Branch> ASMArgBranch::create_clone()
{
    return std::shared_ptr<Branch>(new ASMArgBranch(getCompiler()));
}