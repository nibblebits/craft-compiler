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
 * File:   CallBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 21 June 2016, 21:02
 */

#ifndef CALLBRANCH_H
#define CALLBRANCH_H

#include "CustomBranch.h"
class CallBranch : public CustomBranch {
public:
    CallBranch(Compiler* compiler);
    virtual ~CallBranch();
    std::shared_ptr<Branch> getFunctionNameBranch();
    std::vector<std::shared_ptr<Branch>> getFunctionArgumentBranches();
private:
    
};

#endif /* CALLBRANCH_H */

