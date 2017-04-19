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
 * File:   CraftStaticLibraryLinker.cpp
 * Author: Daniel McCarthy
 *
 * Created on 15 April 2017, 02:39
 * 
 * Description: 
 */

#include "CraftStaticLibraryLinker.h"
#include "CraftStaticLibraryFormat.h"

CraftStaticLibraryLinker::CraftStaticLibraryLinker(Compiler* compiler) : Linker(compiler)
{
}

CraftStaticLibraryLinker::~CraftStaticLibraryLinker()
{
}

/*
 * The "link" method has been overridden as traditional linking will not work for creating this library file,
 * other internal methods will not be invoked automatically due to this overriding.
 */
void CraftStaticLibraryLinker::link()
{
    std::shared_ptr<CraftStaticLibraryFormat> craft_static_lib 
            = std::shared_ptr<CraftStaticLibraryFormat>(new CraftStaticLibraryFormat(getCompiler()));
    
    // Lets add all the current objects to link to this static library
    for (std::shared_ptr<VirtualObjectFormat> obj_format : this->obj_stack)
    {
         craft_static_lib->addObjectFormat(obj_format);  
    }
    
    craft_static_lib->finalize();
    
    // Lets write the object into the executable stream
    getExecutableStream()->writeStream(craft_static_lib->getObjectStream());
}

void CraftStaticLibraryLinker::resolve(std::shared_ptr<VirtualObjectFormat> final_obj)
{

}

void CraftStaticLibraryLinker::build(Stream* executable_stream, std::shared_ptr<VirtualObjectFormat> final_obj)
{

}