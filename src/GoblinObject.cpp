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
 * File:   GoblinObject.cpp
 * Author: Daniel McCarthy
 *
 * Created on 30 June 2016, 05:42
 * 
 * Description: Holds a goblin binary object as well as provides methods for loading, setting and writing the object.
 */

#include "GoblinObject.h"

GoblinObject::GoblinObject()
{
    header.code_generator_descriptor = "NO_GEN";
    header.code_size = 0;
}

GoblinObject::~GoblinObject()
{
}

goblin_obj_header::goblin_obj_header()
{
    this->code_size = 0;
}

goblin_obj_header::~goblin_obj_header()
{

}

goblin_function_calls_header::goblin_function_calls_header()
{
    this->total_func_calls = 0;
    this->func_calls = NULL;
}

goblin_function_calls_header::~goblin_function_calls_header()
{
    if (this->func_calls != NULL)
    {
        delete this->func_calls;
    }
}

goblin_function_call::goblin_function_call()
{
    this->arguments = NULL;
}

goblin_function_call::~goblin_function_call()
{
    if (this->arguments != NULL)
    {
        delete this->arguments;
    }
}

goblin_function_declaration::goblin_function_declaration()
{
    this->arguments = NULL;
}

goblin_function_declaration::~goblin_function_declaration()
{
    if (this->arguments != NULL)
    {
        delete this->arguments;
    }
}

goblin_functions_declaration_header::goblin_functions_declaration_header()
{
    this->functions = NULL;
}

goblin_functions_declaration_header::~goblin_functions_declaration_header()
{
    if (this->functions != NULL)
    {
        delete this->functions;
    }
}


/* Writes the object information to the stream*/
void GoblinObject::write()
{
    this->stream.writeStr(header.descriptor);
    this->stream.writeStr(header.code_generator_descriptor);

    struct goblin_functions_declaration_header* func_declarations_header = &header.func_declarations_header;
    this->stream.write32(func_declarations_header->t_functions);
    for (int i = 0; i < func_declarations_header->t_functions; i++)
    {
        struct goblin_function_declaration* func_declaration = &func_declarations_header->functions[i];
        this->stream.writeStr(func_declaration->func_name);
        this->stream.write32(func_declaration->t_arguments);
        for (int f = 0; f < func_declaration->t_arguments; f++)
        {
            struct goblin_argument* argument = &func_declaration->arguments[f];
            this->stream.writeStr(argument->type);
        }
        
        this->stream.write32(func_declaration->mem_pos);
    }
    struct goblin_function_calls_header* func_calls_header = &header.func_calls_header;
    this->stream.write32(func_calls_header->total_func_calls);
    for (int i = 0; i < func_calls_header->total_func_calls; i++)
    {
        struct goblin_function_call* func_call = &func_calls_header->func_calls[i];
        this->stream.writeStr(func_call->func_name);
        this->stream.write32(func_call->t_arguments);
        for (int a = 0; a < func_call->t_arguments; a++)
        {
            struct goblin_argument* argument = &func_call->arguments[a];
            this->stream.writeStr(argument->type);
        }
    }

    this->stream.write32(header.code_size);
    while (!header.code.isEmpty())
    {
        this->stream.write8(header.code.read8());
    }
}

/* Reads data from an input stream into this goblin object*/
void GoblinObject::read(std::ifstream ifs)
{

}

Stream* GoblinObject::getStream()
{
    return &stream;
}