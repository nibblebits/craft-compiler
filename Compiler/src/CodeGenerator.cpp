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
 * File:   CodeGenerator.cpp
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 19:49
 * 
 * Description: The base class for all code generators.
 * 
 * Code generators are objects that take in an AST(Abstract Syntax Tree) and convert it to some form of output
 * weather that be byte code, machine code or even source code.
 */

#include "CodeGenerator.h"
#include "branches.h"

CodeGenerator::CodeGenerator(Compiler* compiler, std::string code_gen_desc) : CompilerEntity(compiler)
{
    this->stream = new Stream();
}

CodeGenerator::~CodeGenerator()
{
    delete this->stream;
}
void CodeGenerator::generate(std::shared_ptr<Tree> tree)
{
    // Generate all the global branches, the "generate_global_branch" method should handle their children
    std::shared_ptr<Branch> root = tree->root;
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        generate_global_branch(branch);
    }
    
    // Assemble it all together
    assemble(this->assembly);
}

void CodeGenerator::do_asm(std::string asm_ins)
{
    this->assembly += asm_ins + "\n";
}

Stream* CodeGenerator::getStream()
{
    return this->stream;
}