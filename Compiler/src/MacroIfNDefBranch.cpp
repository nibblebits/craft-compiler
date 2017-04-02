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
 * File:   MacroIfNDefBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 02 April 2017, 02:12
 * 
 * Description: 
 */

#include "MacroIfNDefBranch.h"

MacroIfNDefBranch::MacroIfNDefBranch(Compiler* compiler) : MacroStmtExpBodyBranch(compiler, "MACRO_IFNDEF", "")
{
}

MacroIfNDefBranch::~MacroIfNDefBranch()
{
}

void MacroIfNDefBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    MacroStmtExpBodyBranch::imp_clone(cloned_branch);
}

std::shared_ptr<Branch> MacroIfNDefBranch::create_clone()
{
    return std::shared_ptr<MacroIfNDefBranch>(new MacroIfNDefBranch(getCompiler()));
}