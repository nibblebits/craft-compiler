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
 * File:   Argument.h
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 23:55
 */

#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <string>
class __declspec(dllexport) Argument {
public:
    Argument();
    virtual ~Argument();
    std::string name;
    std::string value;
private:
    
};

#endif /* ARGUMENT_H */

