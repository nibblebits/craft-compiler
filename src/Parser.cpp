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
 * 
 */

#include "Parser.h"
#include "branches.h"

Parser::Parser(Compiler* compiler) : CompilerEntity(compiler)
{
    this->tree = std::shared_ptr<Tree>(new Tree());
    this->look_ahead = NULL;
    this->last_matching_rule = NULL;
    this->reduce_position = -1;
}

Parser::~Parser()
{

}

void Parser::addRule(std::string rule_exp)
{
    bool branchable = true;
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

    if (split[0][0] == '\'')
    {
        branchable = false;
        split[0] = Helper::str_remove(split[0], 0);
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

    if (!branchable)
    {
        rule->canCreateBranch(false);
    }
    this->rules.push_back(rule);
}

void Parser::setInput(std::vector<std::shared_ptr<Token>> tokens)
{
    // Push the tokens to the input stack.
    for (std::shared_ptr<Token> token : tokens)
    {
        this->input.push(token);
    }
}

void Parser::shift()
{
    this->parse_stack.push(this->look_ahead);
    if (!this->input.isEmpty())
    {
        this->look_ahead = this->input.pop();
    }
    else
    {
        this->look_ahead = NULL;
    }
}

void Parser::reduce(std::shared_ptr<ParserRule> rule)
{
    std::shared_ptr<Branch> root(new Branch(rule->getName(), ""));
    std::vector<std::shared_ptr < ParserRuleRequirement>> requirements = rule->getRequirements();
    for (int i = 0; i < requirements.size(); i++)
    {
        root->addChild(this->parse_stack.pop());
    }

    // Finally add the root back to the parse stack
    this->parse_stack.push(root);
}

void Parser::tryToReduce()
{
    for (std::shared_ptr<ParserRule> rule : this->rules)
    {
        int matched = 0;
        Stack<std::shared_ptr < Branch>> stack_cpy = this->parse_stack;
        std::vector<std::shared_ptr < ParserRuleRequirement>> requirements = rule->getRequirements();
        for (std::shared_ptr<ParserRuleRequirement> rule_req : requirements)
        {
            // Empty stack we cannot do anything here. Maybe the next rule will be different?
            if (stack_cpy.isEmpty())
            {
                break;
            }
            
            std::shared_ptr<Branch> branch = stack_cpy.pop();
            if (rule_req->getClassName() == branch->getType()
                    && rule_req->allowed(branch->getValue()))
            {
                matched++;
            }
            else
            {
                break;
            }
        }
        if (matched == requirements.size())
        {
            // Rule has matched!
            this->reduce(rule);
        }
    }
}

void Parser::buildTree()
{
    if (this->input.isEmpty())
    {
        throw ParserException("No input given to parser");
    }

    this->look_ahead = this->input.pop();

    while (true)
    {
        shift();
        tryToReduce();

        if (this->input.isEmpty() && this->look_ahead == NULL)
        {
            break;
        }

    }

    this->tree->root = std::shared_ptr<Branch>(new Branch("root", ""));
    this->tree->root->addChild(this->parse_stack.pop());
}

std::shared_ptr<Tree> Parser::getTree()
{
    return this->tree;
}