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
 * File:   STRUCTAccess.cpp
 * Author: Daniel McCarthy
 *
 * Created on 17 October 2016, 03:18
 * 
 * Description: 
 */

#include "STRUCTAccessBranch.h"
#include "STRUCTDEFBranch.h"
#include "VarIdentifierBranch.h"

STRUCTAccessBranch::STRUCTAccessBranch(Compiler* compiler) : CustomBranch(compiler, "STRUCT_ACCESS", "")
{
    this->struct_def_branch = NULL;
}

STRUCTAccessBranch::~STRUCTAccessBranch()
{
}

void STRUCTAccessBranch::setVarIdentifierBranch(std::shared_ptr<VarIdentifierBranch> var_iden_branch)
{
    CustomBranch::registerBranch("var_iden_branch", var_iden_branch);
}

void STRUCTAccessBranch::setAccessAsPointer(bool access_as_pointer)
{
    this->access_as_pointer = access_as_pointer;
}

void STRUCTAccessBranch::setStructDefBranch(std::shared_ptr<STRUCTDEFBranch> struct_def_branch)
{
    this->struct_def_branch = struct_def_branch;
}

std::shared_ptr<VarIdentifierBranch> STRUCTAccessBranch::getVarIdentifierBranch()
{
    return std::dynamic_pointer_cast<VarIdentifierBranch>(CustomBranch::getRegisteredBranchByName("var_iden_branch"));
}

bool STRUCTAccessBranch::isAccessingAsPointer()
{
    return this->access_as_pointer;
}

std::shared_ptr<STRUCTDEFBranch> STRUCTAccessBranch::getStructDefBranch()
{
    return this->struct_def_branch;
}

void STRUCTAccessBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<STRUCTAccessBranch> s_access_branch_cloned = std::dynamic_pointer_cast<STRUCTAccessBranch>(cloned_branch);
    s_access_branch_cloned->setVarIdentifierBranch(std::dynamic_pointer_cast<VarIdentifierBranch>(getVarIdentifierBranch()->clone()));
    s_access_branch_cloned->setAccessAsPointer(isAccessingAsPointer());
    
    if (struct_def_branch != NULL)
        s_access_branch_cloned->setStructDefBranch(getStructDefBranch());

}

std::shared_ptr<Branch> STRUCTAccessBranch::create_clone()
{
    return std::shared_ptr<Branch>(new STRUCTAccessBranch(getCompiler()));
}
