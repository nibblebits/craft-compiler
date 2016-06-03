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

}

void Parser::addRule(std::string rule_exp)
{
    std::vector<std::string> split;
    if (rule_exp.find(" ") != -1)
    {
        throw ParserException("Rules may not contain spaces");
    }

    split = Helper::split(rule_exp, ':');
    if (split.size() == 0)
    {
        throw ParserException("Parser rule not formatted correctly");
    }

    std::shared_ptr<ParserRule> rule = std::shared_ptr<ParserRule>(new ParserRule(split[0]));
    for (int i = 1; i < split.size(); i++)
    {
        std::vector<std::string> allowed_values = Helper::split(split[i], '@');
        std::string requirement_name = Helper::str_remove(allowed_values[0], '\'');
        std::shared_ptr<ParserRuleRequirement> requirement = std::shared_ptr<ParserRuleRequirement>(new ParserRuleRequirement(requirement_name));
        for (int i = 1; i < allowed_values.size(); i++)
        {
            std::string allowed_value = allowed_values[i];
            requirement->allow(allowed_value);
        }

        if (split[i][0] == '\'')
        {
            requirement->excludeFromTree(true);
        }

        rule->addRequirement(requirement);
    }
    this->rules.push_back(rule);
}

void Parser::setInput(std::vector<std::shared_ptr<Token>> tokens)
{
    this->input = tokens;
}

int Parser::isPartOfRule(std::shared_ptr<ParserRule> rule, std::shared_ptr<Branch> branch, int pos)
{
    bool ok = false;
    std::vector<std::shared_ptr < ParserRuleRequirement>> requirements = rule->getRequirements();
    if (requirements.size() < pos + 1)
        return PARSER_RULE_INCOMPATIBLE;

    std::shared_ptr<ParserRuleRequirement> requirement = requirements[pos];
    if (requirement->getClassName() == branch->getType())
    {
        if (requirement->hasValueRequirements())
        {
            ok = requirement->allowed(branch->getValue());
        }
        else
        {
            ok = true;
        }

        if (ok)
        {
            if (requirement->excluded())
            {
                return PARSER_RULE_COMPATIBLE_NO_BRANCH;
            }
            else
            {
                return PARSER_RULE_COMPATIBLE;
            }
        }
    }

    return PARSER_RULE_INCOMPATIBLE;
}

void Parser::reductBranches()
{
    for (int i = 0; i < 2000; i++)
    {
        for (std::shared_ptr<ParserRule> rule : this->rules)
        {
            for (int i = 0; i < this->branches.size(); i++)
            {
                std::vector<std::shared_ptr < Branch>> tmp_list;
                int total_requirements = rule->getRequirements().size();

                if (i + total_requirements <= this->branches.size())
                {
                    for (int b = 0; b < total_requirements; b++)
                    {
                        int index = i + b;
                        std::shared_ptr<Branch> branch = this->branches[index];
                        int result = this->isPartOfRule(rule, branch, b);
                        if (result == PARSER_RULE_COMPATIBLE || result == PARSER_RULE_COMPATIBLE_NO_BRANCH)
                        {
                            if (result == PARSER_RULE_COMPATIBLE_NO_BRANCH)
                            {
                                branch->exclude(true);
                            }
                            tmp_list.push_back(branch);
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                if (tmp_list.size() == total_requirements)
                {
                    // We have a match
                    std::shared_ptr<Branch> root = std::shared_ptr<Branch>(new Branch(rule->getName(), ""));
                    for (std::shared_ptr<Branch> branch : tmp_list)
                    {
                        if (!branch->excluded())
                            root->addChild(branch);
                    }

                    // Erase the parsed branches from the vector
                    this->branches.erase(this->branches.begin() + i, this->branches.begin() + i + tmp_list.size());
                    // Add the new branch into the branches list
                    this->branches.insert(this->branches.begin() + i, root);
                }
            }
        }
    }
}

void Parser::buildTree()
{
    if (this->input.size() == 0)
        throw ParserException("No token input has been specified.");

    this->tree = std::shared_ptr<Tree>(new Tree());
    this->tree->root = std::shared_ptr<Branch>(new Branch("root", ""));
    for (std::shared_ptr<Token> token : this->input)
    {
        this->branches.push_back(token);
    }

    reductBranches();

    for (std::shared_ptr<Branch> branch : this->branches)
    {
        this->tree->root->addChild(branch);
    }
}

std::shared_ptr<Tree> Parser::getTree()
{
    return this->tree;
}