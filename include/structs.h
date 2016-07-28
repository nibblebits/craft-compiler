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
 * File:   structs.h
 * Author: dansb
 *
 * Description: Holds any structures that have to be used throughout the project and not just in one instance.
 * Created on 26 July 2016, 00:15
 */

#ifndef STRUCTS_H
#define STRUCTS_H

#include <memory>

class Scope;
struct variable
{
    variable() {}
    virtual ~variable() {}
    std::string type;
    std::string name;
    int size;
    int mem_pos;
    bool is_array;
    std::shared_ptr<Scope> scope;

};

struct array_scope_variable : public variable
{
    array_scope_variable() {}
    virtual ~array_scope_variable() {}
    int size_per_element;
};
#endif /* STRUCTS_H */

