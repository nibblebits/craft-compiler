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

std::shared_ptr<ParserRuleRequirement> ParserRule::getRequirementByIndex(int index)
{
    if (index < this->requirements.size())
    {
        return this->requirements.at(index);
    }

    throw Exception("std::shared_ptr<ParserRuleRequirement> ParserRule::getRequirementByIndex(int index): index breaches vector size.");
}

bool ParserRule::isValid(Stack<std::shared_ptr<Branch>>*stack)
{
    std::vector<std::shared_ptr < ParserRuleRequirement>> requirements = this->getRequirements();
    if (stack->size() <= requirements.size())
    {
        bool ok = true;
        for (int i = 0; i < requirements.size(); i++)
        {
            std::shared_ptr<Branch> branch = stack->getByIndex(i);
            if (!isValid(branch, i))
            {
                ok = false;
                break;
            }
        }

        if (ok)
        {
            return true;
        }
    }

    return false;
}

bool ParserRule::isValid(std::shared_ptr<Branch> branch, int s_pos)
{
    if (s_pos < this->requirements.size())
    {
        std::shared_ptr<ParserRuleRequirement> requirement = this->requirements.at(s_pos);
        if (branch->getType() == requirement->getClassName()
                && requirement->allowed(branch->getValue()))
            return true;
    }
    return false;
}