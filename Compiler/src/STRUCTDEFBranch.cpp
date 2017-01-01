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
 * File:   STRUCTDEFBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 14 October 2016, 04:20
 * 
 * Description: 
 */

#include "STRUCTDEFBranch.h"
#include "BODYBranch.h"

STRUCTDEFBranch::STRUCTDEFBranch(Compiler* compiler) : VDEFBranch(compiler, "STRUCT_DEF")
{
}

STRUCTDEFBranch::~STRUCTDEFBranch()
{
}

void STRUCTDEFBranch::setStructBody(std::shared_ptr<BODYBranch> struct_body_branch)
{
    this->unique_struct_body_branch = struct_body_branch;
}

std::shared_ptr<BODYBranch> STRUCTDEFBranch::getStructBody()
{
    return this->unique_struct_body_branch;
}

int STRUCTDEFBranch::getBranchType()
{
    return BRANCH_TYPE_VDEF;
}

void STRUCTDEFBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    VDEFBranch::imp_clone(cloned_branch);
    std::shared_ptr<STRUCTDEFBranch> struct_def_branch_clone =
            std::dynamic_pointer_cast<STRUCTDEFBranch>(cloned_branch);

    if (this->unique_struct_body_branch != NULL)
    {
        struct_def_branch_clone->setStructBody(std::dynamic_pointer_cast<BODYBranch>(this->unique_struct_body_branch->clone()));
    }
}

std::shared_ptr<Branch> STRUCTDEFBranch::create_clone()
{
    return std::shared_ptr<Branch>(new STRUCTDEFBranch(getCompiler()));
}
