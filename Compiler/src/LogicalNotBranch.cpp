/*
    Goblin compiler v1.0 - The standard compiler for the Goblin language.
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
 * File:   LogicalNotBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 17 December 2016, 15:06
 * 
 * Description: 
 */

#include "LogicalNotBranch.h"

LogicalNotBranch::LogicalNotBranch(Compiler* compiler) : CustomBranch(compiler, "LOGICAL_NOT", "")
{
}

LogicalNotBranch::~LogicalNotBranch()
{
}

void LogicalNotBranch::setSubjectBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("subject_branch", branch);
}

std::shared_ptr<Branch> LogicalNotBranch::getSubjectBranch()
{
    return CustomBranch::getRegisteredBranchByName("subject_branch");
}

void LogicalNotBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<LogicalNotBranch> logical_not_branch = std::dynamic_pointer_cast<LogicalNotBranch>(cloned_branch);
    logical_not_branch->setSubjectBranch(getSubjectBranch()->clone());
}

std::shared_ptr<Branch> LogicalNotBranch::create_clone()
{
    return std::shared_ptr<Branch>(new LogicalNotBranch(getCompiler()));
}