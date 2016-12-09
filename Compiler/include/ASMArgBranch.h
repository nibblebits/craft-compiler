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
 * File:   ASMArgBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 01 December 2016, 10:09
 */

#ifndef ASMARGBRANCH_H
#define ASMARGBRANCH_H

#include "CustomBranch.h"

class EXPORT ASMArgBranch : public CustomBranch
{
public:
    ASMArgBranch(Compiler* compiler);
    virtual ~ASMArgBranch();

    void setArgumentValueBranch(std::shared_ptr<Branch> arg_value_branch);
    void setNextStringBranch(std::shared_ptr<Branch> next_string_branch);

    std::shared_ptr<Branch> getArgumentValueBranch();
    std::shared_ptr<Branch> getNextStringBranch();
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();
private:

};

#endif /* ASMARGBRANCH_H */

