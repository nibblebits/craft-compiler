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
 * File:   GoblinObjectLinker.cpp
 * Author: Daniel McCarthy
 *
 * Created on 02 July 2016, 03:38
 * 
 * Description: The class links goblin objects.
 */

#include "GoblinObjectLinker.h"
#include "GoblinObject.h"

GoblinObjectLinker::GoblinObjectLinker(Compiler* compiler) : Linker(compiler)
{
}

GoblinObjectLinker::~GoblinObjectLinker()
{

}

void GoblinObjectLinker::link_merge(Stream* obj1, Stream* obj2, Stream* result_obj)
{
    GoblinObject gob_obj1;
    GoblinObject gob_obj2;
    GoblinObject gob_result_obj;
    gob_obj1.read(obj1);
    gob_obj2.read(obj2);
   
    this->link_merge(&gob_obj1, &gob_obj2, &gob_result_obj);

    // Write the new stream to the resulting object stream
    Stream* gob_result_obj_stream = gob_result_obj.getStream();
    while(!gob_result_obj_stream->isEmpty())
    {
        result_obj->write8(gob_result_obj_stream->read8());
    }
    
}

void GoblinObjectLinker::final_merge(Stream* executable_stream, Stream* final_obj)
{
    GoblinObject gob_obj;
    gob_obj.read(final_obj);
    // Write the goblin object to the goblin object stream
    gob_obj.write();
    this->final_merge(executable_stream, &gob_obj);
}
