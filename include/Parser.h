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
 * File:   Parser.h
 * Author: Daniel McCarthy
 *
 * Created on 29 May 2016, 20:31
 */

#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <deque>
#include <memory>
#include <algorithm>
#include "Logger.h"
#include "Helper.h"
#include "ParserRule.h"
#include "ParserRuleRequirement.h"
#include "ParserException.h"
#include "Tree.h"
#include "Token.h"
#include "Branch.h"
#include "CompilerEntity.h"

#define PARSER_RULE_COMPATIBLE 0
#define PARSER_RULE_COMPATIBLE_NO_BRANCH 1
#define PARSER_RULE_INCOMPATIBLE 2

class CompilerEntity;

class Parser : public CompilerEntity
{
public:
    Parser(Compiler* compiler);
    virtual ~Parser();
    void setInput(std::vector<std::shared_ptr<Token>> tokens);
    void buildTree();
    std::shared_ptr<Tree> getTree();
    std::shared_ptr<Logger> getLogger();
private:
    void process_top();
    void process_function();
    void process_body();
    void process_stmt();
    void process_variable_declaration();
    void process_assignment();
    void error(std::string message, bool token = true);
    void warn(std::string message, bool token = true);
    void error_unexpected_token();
    void error_expecting(std::string expecting, std::string given);
    void shift();
    void peak(int offset = -1);
    void pop_branch();
    void push_branch(std::shared_ptr<Branch> branch);
    void shift_pop();
    inline bool is_branch_symbol(std::string symbol);
    inline bool is_peak_symbol(std::string symbol);

    std::shared_ptr<Logger> logger;
    std::deque<std::shared_ptr<Token>> input;
    std::deque<std::shared_ptr<Branch>> branches;
    std::shared_ptr<Token> token;
    std::string token_type;
    std::string token_value;

    std::shared_ptr<Token> peak_token;
    std::string peak_token_type;
    std::string peak_token_value;

    std::shared_ptr<Branch> branch;
    std::string branch_type;
    std::string branch_value;

    std::shared_ptr<Tree> tree;
};

#endif /* PARSER_H */

