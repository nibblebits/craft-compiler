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
}

ParserRule::~ParserRule()
{
    // Delete all the rule requirements
    for (ParserRuleRequirement* requirement : this->requirements)
    {
        delete requirement;
    }
}

void ParserRule::addRequirement(ParserRuleRequirement* requirement)
{
    this->requirements.push_back(requirement);
}

std::vector<ParserRuleRequirement*> ParserRule::getRequirements()
{
    return this->requirements;
}

