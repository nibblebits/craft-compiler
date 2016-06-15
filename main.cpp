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
#include <fstream>
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
    if (argc == 1) {
        std::cout << "No arguments provided, a source file must be specified" << std::endl;
        return 1;
    }
    
    // Load the file
    std::ifstream ifs;
    std::string source = "";
    ifs.open(argv[1]);
    if (!ifs.is_open()) {
        std::cout << "Failed to open: " << argv[1] << std::endl;
        return 2;
    }
    
    while(ifs.good()) {
        source += ifs.get();
    }
    ifs.close();
    
    lexer.setInput(source);
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
        return 3;
    }

    if (lexer.getTokens().size() == 0) {
        std::cout << "Nothing to compile, file is empty or just whitespaces." << std::endl;
        return 4;
    }
    
    try
    {
        parser.addRule("E:identifier");
        parser.addRule("E:number");
        parser.addRule("E:E:operator:E");
        parser.addRule("V_DEF:keyword:E");
        parser.addRule("E:'symbol@(:E:'symbol@)");
        parser.addRule("E:E:'symbol@,:E");
        parser.addRule("ASSIGN:E:symbol@=:E");
        parser.addRule("ASSIGN:E:symbol@=:CALL");
        parser.addRule("CALL:'symbol@#:E:E");
        parser.addRule("CALL:'symbol@#:E:ZERO_ARGS");
        parser.addRule("ZERO_ARGS:'symbol@(:'symbol@)");
        parser.setInput(lexer.getTokens());
        parser.buildTree();

#ifdef DEBUG_MODE
        debug_output_branch(parser.getTree()->root);
#endif
    }
    catch (ParserException ex)
    {
        std::cout << "Error parsing: " << ex.getMessage() << std::endl;
        return 5;
    }



    return 0;
}

