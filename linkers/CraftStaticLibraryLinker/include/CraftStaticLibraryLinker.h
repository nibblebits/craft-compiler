/*
    Craft compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   CraftStaticLibraryLinker.h
 * Author: Daniel McCarthy
 *
 * Created on 15 April 2017, 02:39
 */

#ifndef CRAFTSTATICLIBRARYLINKER_H
#define CRAFTSTATICLIBRARYLINKER_H

#include "Compiler.h"
#include "Linker.h"
#include "VirtualObjectFormat.h"

class CraftStaticLibraryLinker : public Linker {
public:
    CraftStaticLibraryLinker(Compiler* compiler);
    virtual ~CraftStaticLibraryLinker();
    virtual void link();
protected:
    virtual void resolve(std::shared_ptr<VirtualObjectFormat> final_obj);
    virtual void build(Stream* executable_stream, std::shared_ptr<VirtualObjectFormat> final_obj);
private:


};

#endif /* CRAFTSTATICLIBRARYLINKER_H */

