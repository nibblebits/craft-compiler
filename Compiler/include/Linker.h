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
 * File:   Linker.h
 * Author: Daniel McCarthy
 *
 * Created on 02 July 2016, 03:20
 */

#ifndef LINKER_H
#define LINKER_H

#include <vector>
#include <fstream>
#include "Stack.h"
#include "Stream.h"
#include "Exception.h"
#include "LinkerException.h"
#include "def.h"
#include "CompilerEntity.h"

class EXPORT Linker : public CompilerEntity {
public:
    Linker(Compiler* compiler);
    virtual ~Linker();
    void addObjectFile(std::string filename);
    void addObjectFileStream(Stream* stream);
    void link();
    Stream* getExecutableStream();
    virtual void link_merge(Stream* obj1, Stream* obj2, Stream* result_obj) = 0;
    virtual void final_merge(Stream* executable_stream, Stream* final_obj) = 0;
private:
    Stack<Stream> obj_stream_stack;
    Stream executable_stream;
};

#endif /* LINKER_H */

