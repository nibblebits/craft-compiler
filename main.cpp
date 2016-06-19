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
#include "Compiler.h"
#include "Exception.h"
#include "GoblinLibraryLoader.h"
#include "GoblinArgumentParser.h"

using namespace std;

enum
{
    NO_ARGUMENTS_PROVIDED = 1,
    ARGUMENT_PARSE_PROBLEM = 2,
    PROBLEM_WITH_ARGUMENT = 3,
    SOURCE_FILE_LOAD_FAILURE = 4,
    ERROR_WITH_LEXER = 5,
    ERROR_WITH_PARSER = 6,
    ERROR_WITH_TYPE_CHECKER = 7
} CompilerErrorCode; 

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

std::string LoadFile(std::string filename)
{
    // Load the file
    std::ifstream ifs;
    std::string source = "";
    ifs.open(filename);
    if (!ifs.is_open())
    {
        throw Exception("Failed to open: " + filename);
    }

    while (ifs.good())
    {
        source += ifs.get();
    }
    ifs.close();

    return source;
}

int main(int argc, char** argv)
{
    std::string codegen;
    std::string input_file;
    std::string output_file;
    std::string source_file_data;
        
    Compiler compiler;
    Lexer* lexer = compiler.getLexer();
    Parser* parser = compiler.getParser();
    TypeChecker* typeChecker = compiler.getTypeChecker();

    std::cout << COMPILER_FULLNAME << std::endl;
    if (argc == 1)
    {
        std::cout << "No arguments provided, provide the argument \"-help\" for more information" << std::endl;
        return NO_ARGUMENTS_PROVIDED;
    }
    else
    {
        try
        {
           ArgumentContainer arguments = GoblinArgumentParser_GetArguments(argc, argv);
           if (!arguments.hasArgument("input"))
           {
               std::cout << "You must provide an input file, use -input filename" << std::endl;
               return PROBLEM_WITH_ARGUMENT;
           }
           
           if (!arguments.hasArgument("output"))
           {
               std::cout << "You must provide an output file, use -output filename" << std::endl;
               return PROBLEM_WITH_ARGUMENT;
           }
           
           if (!arguments.hasArgument("codegen"))
           {
               std::cout << "No code generator provided, defaulting to standard code generator" << std::endl;
               codegen = "goblin_bytecode";
           }
           else
           {
               codegen = arguments.getArgumentValue("codegen");
           }
           
           input_file = arguments.getArgumentValue("input");
           output_file = arguments.getArgumentValue("output");
           
           if (input_file == output_file)
           {
               std::cout << "The input file and the output file may not be the same" << std::endl;
               return PROBLEM_WITH_ARGUMENT;
           }
           std::cout << "Compiling: " << input_file << " to " << output_file << ", code generator: " << codegen << std::endl;
        } catch(GoblinArgumentException ex)
        {
            std::cout << "Error parsing arguments: " + ex.getMessage() << std::endl;
            return ARGUMENT_PARSE_PROBLEM;
        }
    }
    try 
    {
        source_file_data = LoadFile(input_file);
    } catch(Exception ex)
    {
        std::cout << "Problem loading source file: " << ex.getMessage() << std::endl;
        return SOURCE_FILE_LOAD_FAILURE;
    }
    
    lexer->setInput(source_file_data);
    try
    {
        lexer->tokenize();
#ifdef DEBUG_MODE
        debug_output_tokens(lexer->getTokens());
#endif
    }
    catch (LexerException ex)
    {
        std::cout << "Error with input: " << ex.getMessage() << std::endl;
        return ERROR_WITH_LEXER;
    }

    if (lexer->getTokens().size() == 0)
    {
        std::cout << "Nothing to compile, file is empty or just whitespaces." << std::endl;
        return ERROR_WITH_LEXER;
    }

    try
    {
        parser->addRule("E:identifier");
        parser->addRule("E:number");
        parser->addRule("E:E:operator:E");
        parser->addRule("V_DEF:keyword:E");
        parser->addRule("E:'symbol@(:E:'symbol@)");
        parser->addRule("E:E:'symbol@,:E");
        parser->addRule("PD:'symbol@(:V_DEF:'symbol@)");
        parser->addRule("FUNC:keyword:'symbol@#:E:PD:SCOPE");
        parser->addRule("ASSIGN:E:symbol@=:E");
        parser->addRule("ASSIGN:E:symbol@=:STMT");
        parser->addRule("CALL:'symbol@#:E:E");
        parser->addRule("CALL:'symbol@#:E:ZERO_ARGS");
        parser->addRule("ZERO_ARGS:'symbol@(:'symbol@)");
        parser->addRule("SCOPE:'symbol@{:STMT:'symbol@}");
        parser->addRule("SCOPE:'symbol@{:'symbol@}");
        parser->addRule("STMT:CALL");
        parser->addRule("STMT:ASSIGN");
        parser->addRule("STMT:V_DEF");
        parser->addRule("STMT:STMT:STMT");
        parser->setInput(lexer->getTokens());
        parser->buildTree();

#ifdef DEBUG_MODE
        debug_output_branch(parser->getTree()->root);
#endif
    }
    catch (ParserException ex)
    {
        std::cout << "Error parsing: " << ex.getMessage() << std::endl;
        return ERROR_WITH_PARSER;
    }


    try
    {
        typeChecker->setTree(parser->getTree());
        typeChecker->validate();
    }
    catch (TypeCheckerException ex)
    {
        std::cout << "Error with types: " << ex.getMessage() << std::endl;
        return ERROR_WITH_TYPE_CHECKER;
    }
    return 0;
}

