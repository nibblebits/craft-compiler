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
 * File:   Exception.h
 * Author: Daniel McCarthy
 *
 * Created on 27 May 2016, 16:52
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>
#include "def.h"

class Exception : public std::logic_error
{
public:

    Exception(std::string message, std::string func_name="") : logic_error(message)
    {
        this->msg = message;
        this->func_name = func_name;
    }

    virtual ~Exception()
    {

    }

    void setMessage(std::string message)
    {
        this->msg = message;
    }
    
    void appendMessage(std::string message, STRING_APPEND_OPTIONS options)
    {
        if (options & STRING_APPEND_START)
        {
            this->msg = message + this->msg;
        }
        else
        {
            this->msg += message;
        }
    }
    std::string getMessage()
    {
        std::string formatted_msg = "";
        if (hasFunctionName())
        {
            formatted_msg = getFunctionName() + ": ";
        }
        
        formatted_msg += this->msg;
        return formatted_msg;
    }
    
    void setFunctionName(std::string func_name)
    {
        this->func_name = func_name;
    }
    
    std::string getFunctionName()
    {
        return this->func_name;
    }
    
    bool hasFunctionName()
    {
        return this->func_name != "";
    }
private:
    std::string msg;
    std::string func_name;
};


#endif /* EXCEPTION_H */

