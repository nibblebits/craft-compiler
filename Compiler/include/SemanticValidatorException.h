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
 * File:   LexerException.h
 * Author: Daniel McCarthy
 *
 * Created on 27 May 2016, 16:52
 */

#ifndef SEMANTICVALIDATOREXCEPTION_H
#define SEMANTICVALIDATOREXCEPTION_H
#include "SourceCodeRelatedException.h"
#include "CharPos.h"

class SemanticValidatorException : public SourceCodeRelatedException
{
public:

    SemanticValidatorException(std::string message) :
    SourceCodeRelatedException(message)
    {
    }
    
    SemanticValidatorException(CharPos position, std::string cause) :
    SourceCodeRelatedException(position, "validation error", cause)
    {

    }
};

#endif /* TYPECHECKEREXCEPTION_H */

