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
#include <iostream>
#include "CallBranch.h"

GoblinByteCodeGenerator::GoblinByteCodeGenerator(Compiler* compiler) : CodeGenerator(compiler)
{
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
    if (branch->getType() == "CALL")
    {
        std::shared_ptr<CallBranch> callBranch = std::dynamic_pointer_cast<CallBranch>(branch);
        std::cout << "Function call name: " << callBranch->getFunctionNameBranch()->getValue() << std::endl;
        for (std::shared_ptr<Branch> branch : callBranch->getFunctionArgumentBranches())
        {
            std::cout << "Argument name: " << branch->getValue() << std::endl;
        }
    }
}