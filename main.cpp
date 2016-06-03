/*
    Goblin compiler - The standard compiler for the Goblin language.
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
 * File:   main.cpp
 * Author: Daniel McCarthy
 *
 * Created on 27 May 2016, 04:08
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <memory>
#include "def.h"
#include "Lexer.h"
#include "Parser.h"

using namespace std;

#ifdef DEBUG_MODE

void debug_output_tokens(std::vector<std::shared_ptr<Token>> tokens)
{
    std::cout << "DEBUG TOKEN OUTPUT" << std::endl;
    for (std::shared_ptr<Token> token : tokens)
    {
        std::cout << "<" << token->getType() << ", " << token->getValue() << "> ";
    }

    std::cout << std::endl;
}

void debug_output_branch(std::shared_ptr<Branch> branch, int no_tabs = 0)
{
    for (int i = 0; i < no_tabs; i++)
    {
        std::cout << "\t";
    }

    std::cout << branch->getType() << ":" << branch->getValue() << std::endl;

    for (std::shared_ptr<Branch> child : branch->getChildren())
        debug_output_branch(child, no_tabs + 1);
}
#endif

Lexer lexer;
Parser parser;

int main(int argc, char** argv)
{
    std::cout << COMPILER_FULLNAME << std::endl;
    lexer.setInput("a + 32 nn");
    try
    {
        lexer.tokenize();
#ifdef DEBUG_MODE
        debug_output_tokens(lexer.getTokens());
#endif
    }
    catch (LexerException ex)
    {
        std::cout << "Error with input: " << ex.getMessage() << std::endl;
        return 1;
    }

    try
    {
        parser.addRule("E:identifier");
        parser.addRule("E:number");
        parser.addRule("E:E:operator:E");
        parser.addRule("E:symbol@(:E:symbol@)");
        parser.setInput(lexer.getTokens());
        parser.buildTree();

#ifdef DEBUG_MODE
        debug_output_branch(parser.getTree()->root);
#endif
    }
    catch (ParserException ex)
    {
        std::cout << "Error parsing: " << ex.getMessage() << std::endl;
    }



    return 0;
}

