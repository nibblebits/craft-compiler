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
 * File:   MacroDefineBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 09 January 2017, 19:55
 * 
 * Description: 
 */

#include "MacroDefineBranch.h"

MacroDefineBranch::MacroDefineBranch(Compiler* compiler) : CustomBranch(compiler, "MACRO_DEFINE", "")
{
}

MacroDefineBranch::~MacroDefineBranch()
{
}

void MacroDefineBranch::setDefinitionNameBranch(std::shared_ptr<Branch> name_branch)
{
    CustomBranch::registerBranch("name_branch", name_branch);
}

void MacroDefineBranch::setDefinitionValueBranch(std::shared_ptr<Branch> value_branch)
{
    CustomBranch::registerBranch("value_branch", value_branch);
}

std::shared_ptr<Branch> MacroDefineBranch::getDefinitionNameBranch()
{
    return CustomBranch::getRegisteredBranchByName("name_branch");
}

std::shared_ptr<Branch> MacroDefineBranch::getDefinitionValueBranch()
{
    return CustomBranch::getRegisteredBranchByName("value_branch");
}

bool MacroDefineBranch::hasDefinitionValueBranch()
{
    return CustomBranch::isBranchRegistered("value_branch");
}

void MacroDefineBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<MacroDefineBranch> macro_define_branch_clone = std::dynamic_pointer_cast<MacroDefineBranch>(cloned_branch);
    macro_define_branch_clone->setDefinitionNameBranch(getDefinitionNameBranch()->clone());
    macro_define_branch_clone->setDefinitionValueBranch(getDefinitionValueBranch()->clone());
}

std::shared_ptr<Branch> MacroDefineBranch::create_clone()
{
    return std::shared_ptr<Branch>(new MacroDefineBranch(getCompiler()));
}