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
 * File:   MacroDefinitionIdentifierBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 20 February 2017, 21:12
 * 
 * Description: 
 */

#include "MacroDefinitionIdentifierBranch.h"

MacroDefinitionIdentifierBranch::MacroDefinitionIdentifierBranch(Compiler* compiler) : CustomBranch(compiler, "MACRO_DEFINITION_IDENTIFIER", "")
{
}

MacroDefinitionIdentifierBranch::~MacroDefinitionIdentifierBranch()
{
}

void MacroDefinitionIdentifierBranch::setIdentifierBranch(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("iden_branch", branch);
}

std::shared_ptr<Branch> MacroDefinitionIdentifierBranch::getIdentifierBranch()
{
    return CustomBranch::getRegisteredBranchByName("iden_branch");
}

void MacroDefinitionIdentifierBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<MacroDefinitionIdentifierBranch> iden_cloned_branch = std::dynamic_pointer_cast<MacroDefinitionIdentifierBranch>(cloned_branch);
    iden_cloned_branch->setIdentifierBranch(getIdentifierBranch()->clone());
}
std::shared_ptr<Branch> MacroDefinitionIdentifierBranch::create_clone()
{
    return std::shared_ptr<MacroDefinitionIdentifierBranch>(new MacroDefinitionIdentifierBranch(getCompiler()));
}
