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
 * File:   SourcecodeRelatedException.h
 * Author: Daniel McCarthy
 *
 * Created on 28 May 2016, 20:11
 * 
 * Description: 
 */

#ifndef SOURCECODERELATEDEXCEPTION_H
#define SOURCECODERELATEDEXCEPTION_H

#include <iostream>
#include "Exception.h"
#include "CharPos.h"

class SourceCodeRelatedException : public Exception
{
public:

    SourceCodeRelatedException(std::string message) :
    Exception(message)
    {
    }

    SourceCodeRelatedException(CharPos position, std::string start_message, std::string cause) :
    Exception(start_message + " on line: " + std::to_string(position.line_no) + ", column: " + std::to_string(position.col_pos) + " cause: " + cause)
    {

    }
};

#endif /* SOURCECODERELATEDEXCEPTION_H */

