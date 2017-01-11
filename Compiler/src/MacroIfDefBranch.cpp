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
 * File:   MacroIfDefBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 08 January 2017, 02:03
 * 
 * Description: 
 */

#include "MacroIfDefBranch.h"
#include "BODYBranch.h"

MacroIfDefBranch::MacroIfDefBranch(Compiler* compiler) : CustomBranch(compiler, "MACRO_IFDEF", "")
{
}

MacroIfDefBranch::~MacroIfDefBranch()
{
}

void MacroIfDefBranch::setRequirementBranch(std::shared_ptr<Branch> requirement_branch)
{
    CustomBranch::registerBranch("requirement_branch", requirement_branch);
}

void MacroIfDefBranch::setBodyBranch(std::shared_ptr<BODYBranch> body_branch)
{
    CustomBranch::registerBranch("body_branch", body_branch);
}

std::shared_ptr<BODYBranch> MacroIfDefBranch::getBodyBranch()
{
    return std::dynamic_pointer_cast<BODYBranch>(CustomBranch::getRegisteredBranchByName("body_branch"));
}

std::shared_ptr<Branch> MacroIfDefBranch::getRequirementBranch()
{
    return CustomBranch::getRegisteredBranchByName("requirement_branch");
}

void MacroIfDefBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<MacroIfDefBranch> ifdef_clone = std::dynamic_pointer_cast<MacroIfDefBranch>(cloned_branch);
    ifdef_clone->setRequirementBranch(getRequirementBranch()->clone());
    ifdef_clone->setBodyBranch(std::dynamic_pointer_cast<BODYBranch>(getBodyBranch()->clone()));
}

std::shared_ptr<Branch> MacroIfDefBranch::create_clone()
{
    return std::shared_ptr<Branch>(new MacroIfDefBranch(getCompiler()));
}