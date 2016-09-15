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
 * Description: Holds a goblin binary object as well as provides methods for loading, setting, writing and reading streams into the object.
 */

#include "GoblinObject.h"
#include "CodeGenerator.h"

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

struct goblin_argument* goblin_function_call::getArgument(int index)
{
    if (index < this->t_arguments)
    {
        return &this->arguments[index];
    }
    
    return NULL;
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

struct goblin_argument* goblin_function_declaration::getArgument(int index)
{
    if (index < this->t_arguments)
    {
        return &this->arguments[index];
    }
    
    return NULL;
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


bool goblin_functions_declaration_header::hasFunction(std::string func_name)
{
    return this->getFunction(func_name) != NULL;
}

struct goblin_function_declaration* goblin_functions_declaration_header::getFunction(std::string func_name)
{
    for (int i = 0; i < this->t_functions; i++)
    {
        struct goblin_function_declaration* func_decl = &this->functions[i];
        if (func_decl->func_name == func_name)
            return func_decl;
    }
    
    return NULL;
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
        this->stream.write32(func_call->mem_pos);
    }

    this->stream.write32(header.code_size);
    /* Must be a copy of the code stream as its possible to link streams directly
    * Failure to make a copy would mean the code stream containing zero elements
     * during link time.
     */
    Stream code_copy = header.code;
    while (!code_copy.isEmpty())
    {
        this->stream.write8(code_copy.read8());
    }
}

/* Reads data from an input stream into this goblin object*/
void GoblinObject::read(std::ifstream ifs)
{
    this->stream.loadFrom_ifstream(&ifs);
    this->read(&this->stream);
}

/* Reads data from a Stream into the goblin object*/
void GoblinObject::read(Stream* stream)
{
    if (stream->readStr() != "GOB")
    {
        throw Exception("This is not a goblin object file");
    }

    struct goblin_functions_declaration_header* func_declaration_header = &header.func_declarations_header;
    struct goblin_function_calls_header* func_calls_header = &header.func_calls_header;
    header.code_generator_descriptor = stream->readStr();
    func_declaration_header->t_functions = stream->read32();
    if (func_declaration_header->t_functions != 0)
    {
        func_declaration_header->functions = new goblin_function_declaration[func_declaration_header->t_functions];
        for (int i = 0; i < func_declaration_header->t_functions; i++)
        {
            struct goblin_function_declaration* func_decl = &func_declaration_header->functions[i];
            func_decl->func_name = stream->readStr();
            func_decl->t_arguments = stream->read32();
            if (func_decl->t_arguments != 0)
            {
                func_decl->arguments = new goblin_argument[func_decl->t_arguments];
                for (int a = 0; a < func_decl->t_arguments; a++)
                {
                    struct goblin_argument* gob_argument = &func_decl->arguments[a];
                    gob_argument->type = stream->readStr();
                }
            }
            func_decl->mem_pos = stream->read32();
        }
    }

    func_calls_header->total_func_calls = stream->read32();
    if (func_calls_header->total_func_calls != 0)
    {
        func_calls_header->func_calls = new goblin_function_call[func_calls_header->total_func_calls];
        for (int i = 0; i < func_calls_header->total_func_calls; i++)
        {
            struct goblin_function_call* func_calls = &func_calls_header->func_calls[i];
            func_calls->func_name = stream->readStr();
            func_calls->t_arguments = stream->read32();
            if (func_calls->t_arguments != 0)
            {
                func_calls->arguments = new goblin_argument[func_calls->t_arguments];
                for (int a = 0; a < func_calls->t_arguments; a++)
                {
                    struct goblin_argument* argument = &func_calls->arguments[a];
                    argument->type = stream->readStr();
                }
            }
            
            func_calls->mem_pos = stream->read32();
            
        }
    }

    header.code_size = stream->read32();
    Stream* code_stream = &header.code;
    for (int i = 0; i < header.code_size; i++)
    {
        code_stream->write8(stream->read8());
    }
}

Stream* GoblinObject::getStream()
{
    return &stream;
}