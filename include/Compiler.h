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
 * File:   Compiler.h
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 18:53
 */

#ifndef COMPILER_H
#define COMPILER_H

#include "Lexer.h"
#include "Parser.h"
#include "TypeChecker.h"
#include "ASTAssistant.h"

class Compiler {
public:
    Compiler();
    virtual ~Compiler();
    Lexer* getLexer();
    Parser* getParser();
    TypeChecker* getTypeChecker();
    ASTAssistant* getASTAssistant();
private:
    /* Ideally these do not have to be pointers but since arguments now exist in the constructors of these objects I have ran into issues calling them.
     * I plan to change this soon */
    Lexer* lexer;
    Parser* parser;
    TypeChecker* typeChecker;
    ASTAssistant* astAssistant;
};

#endif /* COMPILER_H */
