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
 * File:   ExternBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 30 January 2017, 00:37
 * 
 * Description: 
 */

#include "ExternBranch.h"

ExternBranch::ExternBranch(Compiler* compiler) : CustomBranch(compiler, "EXTERN", "")
{
}

ExternBranch::~ExternBranch()
{
}

void ExternBranch::setNameBranch(std::shared_ptr<Branch> name_branch)
{
    CustomBranch::registerBranch("name_branch", name_branch);
}

std::shared_ptr<Branch> ExternBranch::getNameBranch()
{
    return CustomBranch::getRegisteredBranchByName("name_branch");
}

void ExternBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<ExternBranch> extern_cloned = std::dynamic_pointer_cast<ExternBranch>(cloned_branch);
    extern_cloned->setNameBranch(getNameBranch()->clone());
}

std::shared_ptr<Branch> ExternBranch::create_clone()
{
    return std::shared_ptr<Branch>(new ExternBranch(getCompiler()));
}