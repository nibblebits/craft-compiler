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
 * File:   ContinueBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 10 January 2017, 03:04
 * 
 * Description: 
 */

#include "ContinueBranch.h"

ContinueBranch::ContinueBranch(Compiler* compiler) : CustomBranch(compiler, "CONTINUE", "")
{
}

ContinueBranch::~ContinueBranch()
{
}

void ContinueBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    
}

std::shared_ptr<Branch> ContinueBranch::create_clone()
{
    return std::shared_ptr<Branch>(new ContinueBranch(getCompiler()));
}