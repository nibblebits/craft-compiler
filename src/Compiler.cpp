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
 * Description: 
 */

#include "Compiler.h"

Compiler::Compiler() 
{
    this->lexer = new Lexer(this);
    this->parser = new Parser(this);
    this->typeChecker = new TypeChecker(this);
    this->astAssistant = new ASTAssistant(this);
}

Compiler::~Compiler() 
{
    delete this->typeChecker;
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