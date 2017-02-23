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
 * File:   STRUCTDescriptorBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 23 February 2017, 18:20
 * 
 * Description: 
 */

#include "STRUCTDescriptorBranch.h"
#include "STRUCTDEFBranch.h"

STRUCTDescriptorBranch::STRUCTDescriptorBranch(Compiler* compiler) : CustomBranch(compiler, "STRUCT_DESCRIPTOR", "")
{
}

STRUCTDescriptorBranch::STRUCTDescriptorBranch(Compiler* compiler, std::string branch_type, std::string branch_value) : CustomBranch(compiler, branch_type, branch_value)
{

}

STRUCTDescriptorBranch::~STRUCTDescriptorBranch()
{
}

void STRUCTDescriptorBranch::setStructNameBranch(std::shared_ptr<Branch> name_branch)
{
    CustomBranch::registerBranch("struct_name", name_branch);
}

std::shared_ptr<Branch> STRUCTDescriptorBranch::getStructNameBranch()
{
    return CustomBranch::getRegisteredBranchByName("struct_name");
}

void STRUCTDescriptorBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<STRUCTDescriptorBranch> struct_desc_branch 
            = std::dynamic_pointer_cast<STRUCTDescriptorBranch>(cloned_branch);
    
    struct_desc_branch->setStructNameBranch(getStructNameBranch()->clone());
}

std::shared_ptr<Branch> STRUCTDescriptorBranch::create_clone()
{
    return std::shared_ptr<STRUCTDescriptorBranch>(new STRUCTDescriptorBranch(getCompiler(), getType(), getValue()));
}