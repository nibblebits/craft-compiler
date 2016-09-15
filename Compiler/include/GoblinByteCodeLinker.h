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
 * File:   GoblinByteCodeLinker.h
 * Author: Daniel McCarthy
 *
 * Created on 03 July 2016, 01:53
 */

#ifndef GOBLINBYTECODELINKER_H
#define GOBLINBYTECODELINKER_H

#include <string>
#include "def.h"
#include "GoblinObjectLinker.h"

class EXPORT GoblinByteCodeLinker : public GoblinObjectLinker
{
public:
    GoblinByteCodeLinker(Compiler* compiler);
    virtual ~GoblinByteCodeLinker();

    virtual void link_merge(GoblinObject* obj1, GoblinObject* obj2, GoblinObject* result_obj);
    virtual void final_merge(Stream* executable_stream, GoblinObject* final_obj);
private:
    
    void link_merge_part(GoblinObject* obj, GoblinObject* result_obj, GoblinObject* prev_obj = NULL);

};

#endif /* GOBLINBYTECODELINKER_H */

