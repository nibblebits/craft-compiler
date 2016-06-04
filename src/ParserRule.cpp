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
 * File:   ParserRule.cpp
 * Author: Daniel McCarthy
 *
 * Created on 30 May 2016, 02:28
 * 
 * Description: Contains individual rule information for the parser
 */

#include "ParserRule.h"

ParserRule::ParserRule(std::string name)
{
    this->name = name;
    this->branchable = true;
}

ParserRule::~ParserRule()
{

}

void ParserRule::canCreateBranch(bool branchable)
{
    this->branchable = branchable;
}
bool ParserRule::isBranchable()
{
    return this->branchable;
}

std::string ParserRule::getName()
{
    return this->name;
}

void ParserRule::addRequirement(std::shared_ptr<ParserRuleRequirement>requirement)
{
    this->requirements.push_back(requirement);
}

std::vector<std::shared_ptr<ParserRuleRequirement>> ParserRule::getRequirements()
{
    return this->requirements;
}

