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
 * File:   Linker.cpp
 * Author: Daniel McCarthy
 *
 * Created on 02 July 2016, 03:20
 * 
 * Description: The base linker class for all linkers
 */

#include "Linker.h"
#include "Stream.h"
#include "VirtualObjectFormat.h"

Linker::Linker(Compiler* compiler) : CompilerEntity(compiler)
{
}

Linker::~Linker()
{

}

void Linker::addObjectFile(std::shared_ptr<VirtualObjectFormat> obj)
{
    this->obj_stack.push(obj);
}

void Linker::link()
{
    while (!this->obj_stack.empty())
    {
        std::shared_ptr<VirtualObjectFormat> obj1 = this->obj_stack.front();
        this->obj_stack.pop();
        if (!this->obj_stack.empty())
        {
            std::shared_ptr<VirtualObjectFormat> obj2 = this->obj_stack.back();
            this->obj_stack.pop();
            this->link_merge(obj1, obj2);
            // Push the modified object back to the stack
            this->obj_stack.push(obj1);
        }
        else
        {
            // Resolve unknown symbols
            this->resolve(obj1);
            // Build the executable
            this->build(&this->executable_stream, obj1);
        }
    }
}

void Linker::link_merge(std::shared_ptr<VirtualObjectFormat> obj1, std::shared_ptr<VirtualObjectFormat> obj2)
{
    // We need to merge the formats
    obj1->append(obj2);
}

Stream* Linker::getExecutableStream()
{
    return &this->executable_stream;
}