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
#include "branches.h"

Parser::Parser(Compiler* compiler) : CompilerEntity(compiler)
{
    this->tree = NULL;
    this->current_branch_index = 0;
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

std::vector<std::shared_ptr<ParserRule>> Parser::getRulesForNextBranchSequence(int s_index)
{
    std::vector<std::shared_ptr < ParserRule>> compatible_rules;
    for (std::shared_ptr<ParserRule> rule : this->rules)
    {
        for (int i = s_index; i < this->branches.size(); i++)
        {
            int total_requirements = rule->getRequirements().size();
            int total_found = 0;

            if (this->branches.size() < i + total_requirements)
            {
                break;
            }

            for (int b = 0; b < total_requirements; b++)
            {
                int index = i + b;
                std::shared_ptr<Branch> branch = this->branches.at(index);

                int result = this->isPartOfRule(rule, branch, b);
                if (result == PARSER_RULE_COMPATIBLE || result == PARSER_RULE_COMPATIBLE_NO_BRANCH)
                {
                    if (result == PARSER_RULE_COMPATIBLE_NO_BRANCH)
                    {
                        branch->exclude(true);
                    }

                    total_found++;
                }
                else
                {

                    break;
                }
            }


            if (total_found == total_requirements)
            {
                compatible_rules.push_back(rule);
            }
            break;
        }
    }

    return compatible_rules;
}

std::vector<std::shared_ptr<ParserRule>> Parser::getRulesForNextBranchSequence()
{
    // Search for compatible rules in the branches, if none is found then move to the next branch until a compatible rule is found
    std::vector<std::shared_ptr < ParserRule>> compatible_rules;
    for (int i = 0; i < this->branches.size(); i++)
    {
        compatible_rules = this->getRulesForNextBranchSequence(i);
        // If there are compatible rules then break
        if (compatible_rules.size() != 0)
        {
            break;
        }
        this->current_branch_index++;
    }
    return compatible_rules;
}

std::shared_ptr<ParserRule> Parser::getNextValidRule()
{
    std::shared_ptr<ParserRule> valid_rule = NULL;
    for (std::shared_ptr<ParserRule> rule : this->getRulesForNextBranchSequence())
    {
        if (valid_rule == NULL)
        {
            valid_rule = rule;
            continue;
        }

        int valid_rule_req_size = valid_rule->getRequirements().size();
        int rule_req_size = rule->getRequirements().size();
        if (rule_req_size == valid_rule_req_size)
        {
            // This is not working correctly
           // throw ParserException("Problem with rules, two valid rules have the same size. " + rule->getName() + ":" + valid_rule->getName());
        }

        if (rule_req_size > valid_rule_req_size)
        {
            valid_rule = rule;
        }
    }

    return valid_rule;
}

std::vector<std::shared_ptr<Branch>> Parser::getBranches(int s_index, size_t total)
{
    int ind_t = s_index + total;
    if (ind_t > this->branches.size())
    {
        throw ParserException("std::vector<std::shared_ptr<Branch>> Parser::getBranches(size_t total): attempting to get more branches than exist");
    }

    std::vector<std::shared_ptr < Branch>> branches;
    for (int i = s_index; i < ind_t; i++)
    {
        branches.push_back(this->branches.at(i));
    }

    return branches;
}

void Parser::reductBranches()
{
    // We only want one root branch once we have finished parsing
    while (branches.size() != 1)
    {
        std::shared_ptr<ParserRule> rule = this->getNextValidRule();

        if (rule != NULL)
        {
            std::vector<std::shared_ptr < Branch>> rule_branches = this->getBranches(this->current_branch_index, rule->getRequirements().size());

            // We have a match
            std::shared_ptr<Branch> root;
            if (rule->getName() == "CALL")
            {
                root = std::shared_ptr<CallBranch>(new CallBranch(this->getCompiler()));
            }
            else if (rule->getName() == "FUNC")
            {
                root = std::shared_ptr<FuncBranch>(new FuncBranch(this->getCompiler()));
            }
            else if (rule->getName() == "V_DEF")
            {
                root = std::shared_ptr<VDEFBranch>(new VDEFBranch(this->getCompiler()));
            }
            else if (rule->getName() == "ASSIGN")
            {
                root = std::shared_ptr<AssignBranch>(new AssignBranch(this->getCompiler()));
            }
            else if (rule->getName() == "MATH_E")
            {
                root = std::shared_ptr<MathEBranch>(new MathEBranch(this->getCompiler()));
            }
            else if (rule->getName() == "E")
            {
                root = std::shared_ptr<EBranch>(new EBranch(this->getCompiler()));
            }
            else if (rule->getName() == "ASM")
            {
                root = std::shared_ptr<ASMBranch>(new ASMBranch(this->getCompiler()));
            }
            else if (rule->getName() == "ARRAY")
            {
                root = std::shared_ptr<ArrayBranch>(new ArrayBranch(this->getCompiler()));
            }
            else
            {
                root = std::shared_ptr<Branch>(new Branch(rule->getName(), ""));
            }
            // Add the branches in the tmp_list to the branch
            for (std::shared_ptr<Branch> branch : rule_branches)
            {
                branch->setParent(root);
                if (!branch->excluded())
                    root->addChild(branch);
            }

            // Erase the parsed branches from the vector
            this->branches.erase(this->branches.begin() + this->current_branch_index, this->branches.begin() + this->current_branch_index + rule_branches.size());
            // Add the new branch into the branches list
            this->branches.insert(this->branches.begin() + this->current_branch_index, root);
            this->current_branch_index = 0;
        }
        else
        {
            // Find the first element that is still a token and use it as a base for the error
            for (std::shared_ptr<Branch> branch : this->branches)
            {
                if (branch->getBranchType() == BRANCH_TYPE_TOKEN)
                {
                    std::shared_ptr<Token> token = std::dynamic_pointer_cast<Token>(branch);
                    throw ParserException(token->getPosition(), "the token '" + token->getValue() + "' may not be expected or something nearby.");
                }
            }
            break;
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

    for (int i = 0; i < this->branches.size(); i++)
    {
        std::shared_ptr<Branch> branch = this->branches.at(i);
        this->tree->root->addChild(branch);
    }

    cleanTree();
}

/* Cleans the tree of branches who only have one child*/
void Parser::cleanTree()
{
    std::shared_ptr<Branch> root = this->tree->root;
    cleanBranch(root);
}

/* Cleans the tree of branches who have only one child, starting at the branch passed to it.
 * The branch with only one child is replaced by its child.
 */
void Parser::cleanBranch(std::shared_ptr<Branch> branch)
{
    int children_size = branch->getChildren().size();
    if (children_size == 1)
    {
        if (branch->hasParent())
        {
            // Replace this branch with its only child.
            branch->getParent()->replaceChild(branch, branch->getChildren()[0]);
        }
    }

    for (std::shared_ptr<Branch> child : branch->getChildren())
    {
        this->cleanBranch(child);
    }

}

std::shared_ptr<Tree> Parser::getTree()
{
    return this->tree;
}