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
 * File:   Linker.h
 * Author: Daniel McCarthy
 *
 * Created on 02 July 2016, 03:20
 */

#ifndef LINKER_H
#define LINKER_H

#include <vector>
#include <fstream>
#include <deque>
#include <memory>
#include "Stream.h"
#include "Exception.h"
#include "LinkerException.h"
#include "def.h"
#include "CompilerEntity.h"

class VirtualObjectFormat;

class EXPORT Linker : public CompilerEntity
{
public:
    Linker(Compiler* compiler);
    virtual ~Linker();
    void addObjectFile(std::shared_ptr<VirtualObjectFormat> obj);
    bool hasObjectFile(std::shared_ptr<VirtualObjectFormat> obj);
    void link();
    Stream* getExecutableStream();
protected:
    virtual void link_merge(std::shared_ptr<VirtualObjectFormat> obj1, std::shared_ptr<VirtualObjectFormat> obj2);
    virtual void resolve(std::shared_ptr<VirtualObjectFormat> final_obj) = 0;
    virtual void build(Stream* executable_stream, std::shared_ptr<VirtualObjectFormat> final_obj) = 0;
private:
    std::deque<std::shared_ptr<VirtualObjectFormat>> obj_stack;
    Stream executable_stream;
};

#endif /* LINKER_H */

