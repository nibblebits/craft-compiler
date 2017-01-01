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
 * File:   LinkerException.h
 * Author: Daniel McCarthy
 *
 * Created on 03 July 2016, 03:19
 */

#ifndef LINKEREXCEPTION_H
#define LINKEREXCEPTION_H

#include "Exception.h"
class LinkerException : public Exception {
public:
    LinkerException(std::string message) : Exception(message)
    {
        
    }
    virtual ~LinkerException()
    {
        
    }
private:

};

#endif /* LINKEREXCEPTION_H */

