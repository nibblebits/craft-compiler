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

CodeGenerator::CodeGenerator(Compiler* compiler, std::shared_ptr<VirtualObjectFormat> object_format, std::string code_gen_desc, int pointer_size) : CompilerEntity(compiler)
{
    this->pointer_size = pointer_size;
    this->object_format = object_format;
}

CodeGenerator::~CodeGenerator()
{

}

void CodeGenerator::assemble()
{
    // Assemble it all together
    std::string assembly_str = "";
    for (asm_map_it iterator = this->assembly.begin();
            iterator != this->assembly.end(); iterator++)
    {
        struct formatted_segment segment = format_segment(iterator->first);
        assembly_str += segment.start_segment + "\n"
                + iterator->second + segment.end_segment + "\n";
    }
    assemble(assembly_str);
}

void CodeGenerator::generate(std::shared_ptr<Tree> tree)
{
    // Generate all the global branches, the "generate_global_branch" method should handle their children
    std::shared_ptr<Branch> root = tree->root;
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        generate_global_branch(branch);
    }
}

int CodeGenerator::getPointerSize()
{
    return this->pointer_size;
}

std::shared_ptr<VirtualObjectFormat> CodeGenerator::getObjectFormat()
{
    return this->object_format;
}

void CodeGenerator::do_asm(std::string asm_ins, std::string segment)
{
    std::map<std::string, std::string>::const_iterator it = this->assembly.find(segment);
    bool exists = it != this->assembly.end();

    std::string asm_str = "";
    if (exists)
    {
        asm_str = this->assembly[segment];
    }

    asm_str += asm_ins + "\n";

    this->assembly[segment] = asm_str;
    
#ifdef DEBUG_MODE
    // Output the assembly instruction if debug mode is enabled.
    std::cout << asm_ins << std::endl;
#endif
}