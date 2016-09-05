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
#include <queue>
#include <memory>
#include <algorithm>
#include "Stack.h"
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
class Parser : public CompilerEntity {
public:
    Parser(Compiler* compiler);
    virtual ~Parser();
    void addRule(std::string rule_exp);
    void setInput(std::vector<std::shared_ptr<Token>> tokens);
    void buildTree();
    std::shared_ptr<Tree> getTree();
private:
    void shift();
    std::shared_ptr<ParserRule> matchRule(Stack<std::shared_ptr<Branch>> stack);
    bool ruleCheck(std::shared_ptr<ParserRule> rule, Stack<std::shared_ptr<Branch>> stack);
    void reduce(std::shared_ptr<ParserRule> rule);
    void tryToReduce();
    
    Stack<std::shared_ptr<Token>> input;
    Stack<std::shared_ptr<Branch>> parse_stack;
    
    std::shared_ptr<Token> look_ahead;
    
    std::vector<std::shared_ptr<ParserRule>> rules;
    std::shared_ptr<ParserRule> last_matching_rule;
    int reduce_position;
    std::shared_ptr<Tree> tree;
};

#endif /* PARSER_H */

