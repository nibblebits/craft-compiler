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
 * File:   Linker.cpp
 * Author: Daniel McCarthy
 *
 * Created on 02 July 2016, 03:20
 * 
 * Description: The base linker class for all linkers
 */

#include "Linker.h"
#include "Stream.h"

Linker::Linker(Compiler* compiler) : CompilerEntity(compiler)
{
}

Linker::~Linker()
{

}

void Linker::addObjectFile(std::string filename)
{
    Stream stream;
    stream.loadFromFile(filename);
    this->obj_stream_stack.push(stream);
}

void Linker::addObjectFileStream(Stream* stream)
{
    // We will copy this stream into a new stream for now
    Stream new_stream;
    while(!stream->isEmpty())
    {
        new_stream.write8(stream->read8());
    }
    this->obj_stream_stack.push(new_stream);
}

void Linker::link()
{
    while (!this->obj_stream_stack.isEmpty())
    {
        Stream obj1_stream = this->obj_stream_stack.pop();
        if (!this->obj_stream_stack.isEmpty())
        {
            Stream obj2_stream = this->obj_stream_stack.pop();
            Stream result_stream;
            this->link_merge(&obj1_stream, &obj2_stream, &result_stream);
            // Push the modified stream back to the stack
            this->obj_stream_stack.push(result_stream);
        }
        else
        {
            this->final_merge(&this->executable_stream, &obj1_stream);
        }
    }
}

Stream* Linker::getExecutableStream()
{
    return &this->executable_stream;
}