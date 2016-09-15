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
 * File:   CompilerEntity.cpp
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 19:07
 * 
 * Description: All compiler entities extend this for example the lexer, parser, type checker, ASTAssistant and code generators extend this class.
 * This class provides access to the compiler.
 */

#include "CompilerEntity.h"

CompilerEntity::CompilerEntity(Compiler* compiler)
{
    this->compiler = compiler;
}

CompilerEntity::~CompilerEntity()
{
}

Compiler* CompilerEntity::getCompiler()
{
    return this->compiler;
}