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
 * File:   Compiler.cpp
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 18:53
 * 
 * Description: Holds all major entities to the compiler
 */

#include "Compiler.h"

Compiler::Compiler()
{
    this->lexer = new Lexer(this);
    this->parser = new Parser(this);
    this->typeChecker = new TypeChecker(this);
    this->astAssistant = new ASTAssistant(this);
    this->codeGenerator = NULL;
    this->linker = NULL;
}

Compiler::~Compiler()
{
    delete this->typeChecker;
}

void Compiler::setCodeGenerator(std::shared_ptr<CodeGenerator> codegen)
{
    if (this->codeGenerator != NULL)
        throw Exception("An existing code generator has already been set! Only one code generator may be set to the compiler");
    if (codegen == NULL)
        throw Exception("The code generator may not be NULL!");

    this->codeGenerator = codegen;
}

void Compiler::setLinker(std::shared_ptr<Linker> linker)
{
    this->linker = linker;
}

Lexer* Compiler::getLexer()
{
    return this->lexer;
}

Parser* Compiler::getParser()
{
    return this->parser;
}

TypeChecker* Compiler::getTypeChecker()
{
    return this->typeChecker;
}

ASTAssistant* Compiler::getASTAssistant()
{
    return this->astAssistant;
}

std::shared_ptr<CodeGenerator> Compiler::getCodeGenerator()
{
    return this->codeGenerator;
}

std::shared_ptr<Linker> Compiler::getLinker()
{
    return this->linker;
}

int Compiler::getDataTypeSize(std::string type)
{
    /* Note for now data types such as bit and nibble will consume 1 byte, this will hopefully be changed in the future to work at the bit level*/
    if (type == "bit" || type == "nibble" || type == "uint8" || type == "int8")
    {
        return 1;
    }
    else if (type == "uint16" || type == "int16")
    {
        return 2;
    }
    else if (type == "uint32" || type == "int32")
    {
        return 4;
    }
    else if (type == "uint64" || type == "int64")
    {
        return 8;
    }
    else
    {
        return -1;
    }
}

std::string Compiler::getTypeFromNumber(int number)
{
    if (number >= 0)
    {
        if (number <= 0xff)
        {
            return "uint8";
        }
        else if (number <= 0xffff)
        {
            return "uint16";
        }
        else if (number <= 0xffffffff)
        {
            return "uint32";
        }
    }
    else
    {
        if (number >= -0xff)
        {
            return "int8";
        }
        else if (number >= -0xffff)
        {
            return "int16";
        }
        else if (number >= -0xffffffff)
        {
            return "int32";
        }
    }
}

bool Compiler::canCast(std::string type1, std::string type2)
{
    if (type1 == "uint8" || type1 == "int8")
    {
        if (type2 == "uint8" || type2 == "int8" || type2 == "uint16" || type2 == "int16" || type2 == "uint32" || type2 == "int32")
        {
            return true;
        }
    }
    else if (type1 == "uint16" || type1 == "int16")
    {
        if (type2 == "uint32" || type2 == "int32")
        {
            return true;
        }
    }

    return false;
}

bool Compiler::isCompareOperator(std::string value)
{
    if (
            value == "==" ||
            value == "!=" ||
            value == "<=" ||
            value == ">=" ||
            value == "<" ||
            value == ">")
        return true;

    return false;
}

 bool Compiler::isLogicalOperator(std::string value)
 {
     if (
             value == "&&" ||
             value == "||")
         return true;
     
     return false;
 }