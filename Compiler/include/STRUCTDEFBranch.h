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
 * File:   STRUCTDEFBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 14 October 2016, 04:20
 */

#ifndef STRUCTDEFBRANCH_H
#define STRUCTDEFBRANCH_H

#include "VDEFBranch.h"
class EXPORT STRUCTDEFBranch : public VDEFBranch {
public:
    STRUCTDEFBranch(Compiler* compiler);
    virtual ~STRUCTDEFBranch();
private:

};

#endif /* STRUCTDEFBRANCH_H */

