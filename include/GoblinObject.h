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
 * File:   GoblinObject.h
 * Author: Daniel McCarthy
 *
 * Created on 30 June 2016, 05:42
 */

#ifndef GOBLINOBJECT_H
#define GOBLINOBJECT_H
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include "Stream.h"

enum
{
    GOBLIN_CODE_GENERATOR_DESCRIPTOR_SIZE = 20,
    GOBLIN_FUNCTION_NAME_SIZE = 100,
    GOBLIN_TYPE_SIZE = 10
};

struct goblin_argument;
struct goblin_function_call;
struct goblin_function_calls_header;
struct goblin_function_declaration;
struct goblin_functions_declaration_header;
struct goblin_obj_header;

struct goblin_argument
{
    std::string type;
};

struct goblin_function_call
{
    std::string func_name;
    size_t t_arguments;
    struct goblin_argument* arguments;

    goblin_function_call();
    virtual ~goblin_function_call();
};

struct goblin_function_declaration
{
    std::string func_name;
    size_t t_arguments;
    struct goblin_argument* arguments;
    int mem_pos;
    
    goblin_function_declaration();
    virtual ~goblin_function_declaration();
};

struct goblin_functions_declaration_header
{
    size_t t_functions;
    struct goblin_function_declaration* functions;

    goblin_functions_declaration_header();
    virtual ~goblin_functions_declaration_header();
};

struct goblin_function_calls_header
{
    size_t total_func_calls;
    struct goblin_function_call* func_calls;

    goblin_function_calls_header();
    virtual ~goblin_function_calls_header();
};

struct goblin_obj_header
{
    const char* descriptor = {"GOB"};
    std::string code_generator_descriptor;
    struct goblin_functions_declaration_header func_declarations_header; 
    struct goblin_function_calls_header func_calls_header;
    size_t code_size;
    Stream code;

    goblin_obj_header();
    virtual ~goblin_obj_header();

};

class GoblinObject
{
public:
    GoblinObject();
    virtual ~GoblinObject();
    goblin_obj_header header;
    /* Writes the object information to the stream*/
    void write();
    /* Reads data from an input stream into this goblin object*/
    void read(std::ifstream ifs);
    Stream* getStream();
private:
    Stream stream;
};

#endif /* GOBLINOBJECT_H */

