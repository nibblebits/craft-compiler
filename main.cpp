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
#include "CodeGeneratorException.h"

// Built in code generators
#include "GoblinByteCodeGenerator.h"

using namespace std;

enum
{
    NO_ARGUMENTS_PROVIDED = 1,
    ARGUMENT_PARSE_PROBLEM = 2,
    PROBLEM_WITH_ARGUMENT = 3,
    SOURCE_FILE_LOAD_FAILURE = 4,
    CODEGENERATOR_LOAD_PROBLEM = 5,
    ERROR_WITH_LEXER = 6,
    ERROR_WITH_PARSER = 7,
    ERROR_WITH_TYPE_CHECKER = 8,
    ERROR_WITH_OUTPUT_FILE = 9,
    ERROR_WITH_CODEGENERATOR = 10
} CompilerErrorCode;

Compiler compiler;

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

void WriteFile(std::string filename, Stream* stream)
{
    std::ofstream ofs;
    ofs.open(filename, ios::binary);
    if (!ofs.is_open())
    {
        throw Exception("Failed to open: " + filename + " for writing");
    }
    while (!stream->isEmpty())
    {
        ofs << stream->read8();
    }

    ofs.close();
}

std::shared_ptr<CodeGenerator> getCodeGenerator(std::string codegen_name)
{
    std::shared_ptr<CodeGenerator> codegen = NULL;
    // Check for built in code generators
    if (codegen_name == "goblin_bytecode")
    {
        codegen = std::shared_ptr<CodeGenerator>(new GoblinByteCodeGenerator(&compiler));
    }
    else
    {
        // Ok the generator is not a built in code generator so look for a library for it
        throw Exception("The code generator: " + codegen_name + " could not be found");
    }
    return codegen;
}

int main(int argc, char** argv)
{
    std::string codegen_name;
    std::string input_file_name;
    std::string output_file_name;
    std::string source_file_data;

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
                codegen_name = "goblin_bytecode";
            }
            else
            {
                codegen_name = arguments.getArgumentValue("codegen");
            }

            input_file_name = arguments.getArgumentValue("input");
            output_file_name = arguments.getArgumentValue("output");

            if (input_file_name == output_file_name)
            {
                std::cout << "The input file and the output file may not be the same" << std::endl;
                return PROBLEM_WITH_ARGUMENT;
            }
            std::cout << "Compiling: " << input_file_name << " to " << output_file_name << ", code generator: " << codegen_name << std::endl;
        }
        catch (GoblinArgumentException ex)
        {
            std::cout << "Error parsing arguments: " + ex.getMessage() << std::endl;
            return ARGUMENT_PARSE_PROBLEM;
        }
    }
    try
    {
        source_file_data = LoadFile(input_file_name);
    }
    catch (Exception ex)
    {
        std::cout << "Problem loading source file: " << ex.getMessage() << std::endl;
        return SOURCE_FILE_LOAD_FAILURE;
    }

    std::shared_ptr<CodeGenerator> codegen;
    try
    {
        codegen = getCodeGenerator(codegen_name);
        compiler.setCodeGenerator(codegen);
    }
    catch (Exception ex)
    {
        std::cout << "Problem loading code generator: " << ex.getMessage() << std::endl;
        return CODEGENERATOR_LOAD_PROBLEM;
    }

    Lexer* lexer = compiler.getLexer();
    Parser* parser = compiler.getParser();
    TypeChecker* typeChecker = compiler.getTypeChecker();

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
        parser->addRule("E:MATH_E");
        parser->addRule("MATH_E:E:operator:E");
        parser->addRule("V_DEF:keyword:E");
        parser->addRule("E:'symbol@(:E:'symbol@)");
        parser->addRule("E:E:'symbol@,:E");
        parser->addRule("E:E:'symbol@,:STMT");
        parser->addRule("PD:'symbol@(:V_DEF:'symbol@)");
        parser->addRule("FUNC:keyword:'symbol@#:E:PD:SCOPE");
        parser->addRule("ASSIGN:E:symbol@=:E");
        parser->addRule("ASSIGN:E:symbol@=:STMT");
        parser->addRule("ASSIGN:STMT:symbol@=:STMT");
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

    try
    {
        codegen->generate(parser->getTree());
        Stream* stream = codegen->getStream();
        size_t stream_size = stream->getSize();
        if (stream_size == 0)
        {
            throw CodeGeneratorException("No output was generated");
        }

        try
        {
            WriteFile(output_file_name, stream);
        }
        catch (Exception ex)
        {
            std::cout << ex.getMessage() << std::endl;
            return ERROR_WITH_OUTPUT_FILE;
        }
    }
    catch (CodeGeneratorException ex)
    {
        std::cout << "Error with code generator: " << ex.getMessage() << std::endl;
        return ERROR_WITH_CODEGENERATOR;
    }
    return 0;
}

