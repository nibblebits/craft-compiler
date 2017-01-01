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
 * File:   FuncArgumentsBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 17 December 2016, 13:59
 */

#ifndef FUNCARGUMENTSBRANCH_H
#define FUNCARGUMENTSBRANCH_H

#include "StandardScopeBranch.h"

class FuncArgumentsBranch : public StandardScopeBranch
{
public:
    FuncArgumentsBranch(Compiler* compiler);
    virtual ~FuncArgumentsBranch();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:

};

#endif /* FUNCARGUMENTSBRANCH_H */

