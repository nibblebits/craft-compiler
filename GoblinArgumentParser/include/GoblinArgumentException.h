/*
    Goblin Argument Parser v1.0 - A parser for translating program arguments.
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
 * File:   GoblinArgumentException.h
 * Author: Daniel McCarthy
 *
 * Created on 19 June 2016, 00:04
 * 
 * Description: This is the blueprint for goblin argument exceptions.
 */

#ifndef GOBLINARGUMENTEXCEPTION_H
#define GOBLINARGUMENTEXCEPTION_H
class __declspec(dllexport) GoblinArgumentException
{
public:
    GoblinArgumentException(std::string message)
    {
        this->message = message;
    }
    
    virtual ~GoblinArgumentException()
    {
        
    }
    
    std::string getMessage()
    {
        return this->message;
    }
    
private:
    std::string message;
};


#endif /* GOBLINARGUMENTEXCEPTION_H */

