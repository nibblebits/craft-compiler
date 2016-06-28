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
#include "FuncBranch.h"
#include "VDEFBranch.h"

CodeGenerator::CodeGenerator(Compiler* compiler) : CompilerEntity(compiler)
{
    this->stream = new Stream();
    this->current_index = -1;
}

CodeGenerator::~CodeGenerator()
{
    delete this->stream;
}

void CodeGenerator::startRegistrationProcess()
{
    if (this->current_index != -1)
        throw CodeGeneratorException("Registration process has already started");

    this->current_index = this->stream->getSize();
}

void CodeGenerator::registerMemoryLocation(std::shared_ptr<Branch> branch)
{
    if (branch == NULL)
    {
        throw CodeGeneratorException("Attempting to register a memory location for a NULL branch..");
    }

    if (branch->getType() == "FUNC")
    {
        if (isFunctionRegistered(branch->getValue()))
        {
            throw CodeGeneratorException("Attempting to register an already registered branch");
        }
    }
    else
    {
        throw CodeGeneratorException("Cannot register branch type: " + branch->getType() + " as it is unknown how to register this particular branch");
    }

    if (this->current_index == -1)
        throw CodeGeneratorException("You must invoke the startRegistrationProcess() method first");

    struct location loc;
    loc.branch = branch;
    loc.location = this->current_index;
    this->locations.push_back(loc);
    this->current_index = -1;
}

bool CodeGenerator::isFunctionRegistered(std::string func_name)
{
    for (struct location loc : this->locations)
    {
        std::shared_ptr<Branch> branch = loc.branch;
        if (branch->getType() == "FUNC")
        {
            std::shared_ptr<FuncBranch> func_branch = std::dynamic_pointer_cast<FuncBranch>(branch);
            if (func_branch->getFunctionNameBranch()->getValue() == func_name)
            {
                return true;
            }
        }
    }

    return false;
}

int CodeGenerator::getFunctionMemoryLocation(std::string func_name)
{
    for (struct location loc : this->locations)
    {
        std::shared_ptr<Branch> branch = loc.branch;
        if (branch->getType() == "FUNC")
        {
            std::shared_ptr<FuncBranch> func_branch = std::dynamic_pointer_cast<FuncBranch>(branch);
            if (func_branch->getFunctionNameBranch()->getValue() == func_name)
            {
                return loc.location;
            }
        }
    }
    return -1;
}

void CodeGenerator::generate(std::shared_ptr<Tree> tree)
{
    std::shared_ptr<Branch> root = tree->root;
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        this->generateFromBranch(branch);
    }
}

int CodeGenerator::getScopeVariablesSize()
{
    int size = 0;
    for (std::shared_ptr<struct scope_variable> var : this->scope_variables)
    {
        size += var->size;
    }
    return size;
}

void CodeGenerator::registerScopeVariable(std::shared_ptr<Branch> branch)
{
    if (branch->getType() != "V_DEF")
    {
        throw CodeGeneratorException("The branch: " + branch->getType() + " cannot be converted to a scope variable");
    }

    std::shared_ptr<struct scope_variable> variable = std::shared_ptr<struct scope_variable>(new struct scope_variable);
    std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
    variable->name = vdef_branch->getDefinitionNameBranch()->getValue();
    variable->type = vdef_branch->getDefinitionTypeBranch()->getValue();
    variable->size = this->getCompiler()->getDataTypeSize(variable->type);
    variable->index = this->scope_variables.size();
    this->scope_variables.push_back(variable);
}

std::shared_ptr<struct scope_variable> CodeGenerator::getScopeVariable(std::string name)
{
    for (std::shared_ptr<struct scope_variable> variable : this->scope_variables)
    {
        if (variable->name == name)
            return variable;
    }
    
    return NULL;
}

void CodeGenerator::clearScopeVariables()
{
    this->scope_variables.clear();
}

Stream* CodeGenerator::getStream()
{
    return this->stream;
}

