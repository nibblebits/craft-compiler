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
#include "Helper.h"
#include "ParserRule.h"
#include "ParserRuleRequirement.h"
#include "ParserException.h"
#include "Tree.h"
#include "Token.h"
class Parser {
public:
    Parser();
    virtual ~Parser();
    void addRule(std::string rule_exp);
    void setInput(std::vector<Token> tokens);
    void buildTree();
    Tree* getTree();
private:
    std::vector<Token> input;
    std::vector<ParserRule*> rules;
    Tree* tree;
};

#endif /* PARSER_H */

