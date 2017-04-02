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
 * File:   MacroStmtExpBody.cpp
 * Author: Daniel McCarthy
 *
 * Created on 02 April 2017, 01:56
 * 
 * Description: To be inherited by any macro that requires an expression and a body.
 * 
 * For example:
 * 
 * #ifdef TEST
 * {
 * 
 * }
 */

#include "MacroStmtExpBodyBranch.h"
#include "BODYBranch.h"

MacroStmtExpBodyBranch::MacroStmtExpBodyBranch(Compiler* compiler, std::string type, std::string value) : CustomBranch(compiler, type, value)
{
}

MacroStmtExpBodyBranch::~MacroStmtExpBodyBranch()
{
}

void MacroStmtExpBodyBranch::setRequirementBranch(std::shared_ptr<Branch> requirement_branch)
{
    CustomBranch::registerBranch("requirement_branch", requirement_branch);
}

void MacroStmtExpBodyBranch::setBodyBranch(std::shared_ptr<BODYBranch> body_branch)
{
    CustomBranch::registerBranch("body_branch", body_branch);
}

std::shared_ptr<BODYBranch> MacroStmtExpBodyBranch::getBodyBranch()
{
    return std::dynamic_pointer_cast<BODYBranch>(CustomBranch::getRegisteredBranchByName("body_branch"));
}

std::shared_ptr<Branch> MacroStmtExpBodyBranch::getRequirementBranch()
{
    return CustomBranch::getRegisteredBranchByName("requirement_branch");
}

void MacroStmtExpBodyBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<MacroStmtExpBodyBranch> elem = std::dynamic_pointer_cast<MacroStmtExpBodyBranch>(cloned_branch);
    elem->setRequirementBranch(getRequirementBranch()->clone());
    elem->setBodyBranch(std::dynamic_pointer_cast<BODYBranch>(getBodyBranch()->clone()));
}

std::shared_ptr<Branch> MacroStmtExpBodyBranch::create_clone()
{
    return std::shared_ptr<Branch>(new MacroStmtExpBodyBranch(getCompiler(), getType(), getValue()));
}
