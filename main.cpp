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
#include "def.h"
#include "Lexer.h"
#include "Parser.h"

using namespace std;

#ifdef DEBUG_MODE

void debug_output_tokens(std::vector<Token*> tokens)
{
    std::cout << "DEBUG TOKEN OUTPUT" << std::endl;
    for (Token* token : tokens)
    {
        std::cout << "<" << token->getClass() << ", " << token->getValue() << "> ";
    }

    std::cout << std::endl;
}

#endif

Lexer lexer;
Parser parser;

int main(int argc, char** argv)
{
    std::cout << COMPILER_FULLNAME << std::endl;
    lexer.setInput("uint8 c = \"Hello World@@\" 123\n c++;");
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
        parser.addRule("expression:identifier:operator@-@+:identifier");
    }
    catch(ParserException ex)
    {
        std::cout << "Error parsing: " << ex.getMessage() << std::endl;
    }
    
    

    return 0;
}

