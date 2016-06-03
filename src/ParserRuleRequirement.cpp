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
 * File:   ParserRuleRequirement.cpp
 * Author: Daniel McCarthy
 *
 * Created on 30 May 2016, 02:39
 * 
 * Description: Holds methods regarding rule requirements.
 */

#include "ParserRuleRequirement.h"

ParserRuleRequirement::ParserRuleRequirement(std::string _class)
{
    this->_class = _class;
    this->exclude_from_tree = false;
}

ParserRuleRequirement::~ParserRuleRequirement()
{
}

void ParserRuleRequirement::allow(std::string value)
{
    this->allowed_classes.push_back(value);
}

bool ParserRuleRequirement::allowed(std::string value)
{
    if (this->allowed_classes.size() == 0)
    {
        return true;
    }

    for (std::string str : allowed_classes)
    {
        if (str == value)
        {
            return true;
        }
    }

    return false;
}

void ParserRuleRequirement::excludeFromTree(bool exclude)
{
    this->exclude_from_tree = exclude;
}

bool ParserRuleRequirement::hasValueRequirements()
{
    return this->allowed_classes.size() != 0;
}

bool ParserRuleRequirement::excluded()
{
    return this->exclude_from_tree;
}

std::string ParserRuleRequirement::getClassName()
{
    return this->_class;
}

std::vector<std::string> ParserRuleRequirement::getAllowedValues()
{
    return this->allowed_classes;
}