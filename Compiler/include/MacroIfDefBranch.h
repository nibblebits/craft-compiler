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
 * File:   MacroIfDefBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 08 January 2017, 02:03
 */

#ifndef MACROIFDEFBRANCH_H
#define MACROIFDEFBRANCH_H

#include "CustomBranch.h"

#include "MacroStmtExpBodyBranch.h"
class MacroIfDefBranch : public MacroStmtExpBodyBranch
{
public:
    MacroIfDefBranch(Compiler* compiler);
    virtual ~MacroIfDefBranch();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:

};

#endif /* MACROIFDEFBRANCH_H */

