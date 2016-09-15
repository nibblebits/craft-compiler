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

void CodeGen8086::generateFromBranch(std::shared_ptr<Branch> branch)
{
    std::cout << "TESTING" << std::endl;
    std::cout << branch->getValue() << std::endl;
}

std::shared_ptr<Linker> CodeGen8086::getLinker()
{
    return this->linker;
}

void CodeGen8086::scope_start(std::shared_ptr<Branch> branch)
{

}

void CodeGen8086::scope_end(std::shared_ptr<Branch> branch)
{

}

void CodeGen8086::scope_assignment(std::shared_ptr<struct variable> var, std::shared_ptr<Branch> assign_root, std::shared_ptr<Branch> assign_to)
{

}

void CodeGen8086::scope_func_call(std::shared_ptr<Branch> branch, std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments)
{

}

void CodeGen8086::scope_handle_inline_asm(std::shared_ptr<Branch> branch)
{

}