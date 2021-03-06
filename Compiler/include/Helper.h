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
 * File:   Helper.h
 * Author: Daniel McCarthy
 *
 * Created on 30 May 2016, 04:02
 */

#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <vector>
#include "def.h"

class EXPORT Helper
{
public:
    static std::vector<std::string> split(std::string str, char delimiter);
    static std::string str_remove(std::string str, char remove);
    static std::string str_remove(std::string str, int index);
    static std::string str_replace(std::string str, std::string target, std::string replacement);
private:

};

#endif /* HELPER_H */

