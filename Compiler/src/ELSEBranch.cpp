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
 * File:   ELSEBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 04 October 2016, 01:05
 * 
 * Description: 
 */

#include "ELSEBranch.h"
#include "BODYBranch.h"

ELSEBranch::ELSEBranch(Compiler* compiler) : CustomBranch(compiler, "ELSE", "")
{
}

ELSEBranch::~ELSEBranch()
{
}

void ELSEBranch::setBodyBranch(std::shared_ptr<BODYBranch> branch)
{
    CustomBranch::registerBranch("body_branch", branch);
}

std::shared_ptr<BODYBranch> ELSEBranch::getBodyBranch()
{
    return std::dynamic_pointer_cast<BODYBranch>(CustomBranch::getRegisteredBranchByName("body_branch"));
}

void ELSEBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<ELSEBranch> else_branch_cloned = std::dynamic_pointer_cast<ELSEBranch>(cloned_branch);
    else_branch_cloned->setBodyBranch(std::dynamic_pointer_cast<BODYBranch>(getBodyBranch()->clone()));
}

std::shared_ptr<Branch> ELSEBranch::create_clone()
{
    return std::shared_ptr<Branch>(new ELSEBranch(getCompiler()));
}
