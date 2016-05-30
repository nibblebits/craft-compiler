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
 * File:   Parser.cpp
 * Author: Daniel McCarthy
 *
 * Created on 29 May 2016, 20:31
 * 
 * Description: Takes token input and converts it to an AST(Abstract Syntax Tree)
 */

#include "Parser.h"

Parser::Parser()
{
    this->tree = NULL;
}

Parser::~Parser()
{
    if (this->tree != NULL);
    {
        delete this->tree;
    }

    for (ParserRule* rule : this->rules)
    {
        delete rule;
    }
}

void Parser::addRule(std::string rule_exp)
{
    std::vector<std::string> split;
    if (rule_exp.find(" ") != -1)
    {
        throw ParserException("Rules may not contain spaces!");
    }

    split = Helper::split(rule_exp, ':');
    if (split.size() == 0)
    {
        throw ParserException("Parser rule not formatted correctly");
    }

    ParserRule* rule = new ParserRule(split[0]);
    for (int i = 1; i < split.size(); i++)
    {
        std::string requirement_name = split[i];
        ParserRuleRequirement* requirement = new ParserRuleRequirement(requirement_name);
        std::vector<std::string> allowed_values = Helper::split(requirement_name, '@');
        for (int i = 1; i < allowed_values.size(); i++)
        {
            std::string allowed_value = allowed_values[i];
            requirement->allow(allowed_value);
        }

        rule->addRequirement(requirement);
    }
    this->rules.push_back(rule);
}

void Parser::setInput(std::vector<Token> tokens)
{
    this->input = tokens;
}

void Parser::buildTree()
{
    this->tree = new Tree();

}

Tree* Parser::getTree()
{

}