/*
    Goblin compiler - The standard compiler for the Craft programming language.
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
#include <string>
#include "def.h"
#include "Compiler.h"
#include "branches.h"
#include "Exception.h"
#include "SemanticValidatorException.h"
#include "GoblinLibraryLoader.h"
#include "GoblinArgumentParser.h"
#include "CodeGenerator.h"
#include "CodeGeneratorException.h"
#include "Linker.h"
#include "Preprocessor.h"

using namespace std;

typedef CodeGenerator* (*CodegenInitFunc)(Compiler*, std::shared_ptr<VirtualObjectFormat> object_format);
typedef VirtualObjectFormat* (*VirtualObjFormatInitFunc)(Compiler*);

enum
{
    NO_ARGUMENTS_PROVIDED = 1,
    ARGUMENT_PARSE_PROBLEM = 2,
    PROBLEM_WITH_ARGUMENT = 3,
    SOURCE_FILE_LOAD_FAILURE = 4,
    OBJECT_FORMAT_LOAD_PROBLEM = 5,
    CODEGENERATOR_LOAD_PROBLEM = 6,
    ERROR_WITH_LEXER = 7,
    ERROR_WITH_PARSER = 8,
    ERROR_WITH_SEMANTIC_VALIDATION = 9,
    ERROR_WITH_TREE_IMPROVER = 10,
    ERROR_WITH_OUTPUT_FILE = 11,
    ERROR_WITH_CODEGENERATOR = 12,
    ERROR_WITH_PREPROCESSOR = 13,
} CompilerErrorCode;

Compiler compiler;

Lexer* lexer;
Parser* parser;
SemanticValidator* semanticValidator;
TreeImprover* treeImprover;
Preprocessor* preprocessor;

std::shared_ptr<VirtualObjectFormat> getObjectFormat(std::string object_format_name)
{
    std::shared_ptr<VirtualObjectFormat> virtual_obj_format = NULL;
    void* lib_addr = GoblinLoadLibrary(std::string(std::string(OBJ_FORMAT_DIR)
                                                   + "/" + object_format_name + std::string(CODEGEN_EXT)).c_str());
    if (lib_addr == NULL)
    {
        throw Exception("The object format: " + object_format_name + " could not be found or loaded");
    }

    VirtualObjFormatInitFunc init_func = (VirtualObjFormatInitFunc) GoblinGetAddress(lib_addr, "Init");
    if (init_func == NULL)
    {
        throw Exception("The virtual object format: " + object_format_name + " does not have a valid \"Init\" function");
    }

    virtual_obj_format = std::shared_ptr<VirtualObjectFormat>(init_func(&compiler));

    if (virtual_obj_format == NULL)
    {
        throw Exception("The virtual object format: " + object_format_name + " returned a NULL pointer when expecting a \"VirtualObjectFormat\" object");
    }

    return virtual_obj_format;
}

std::shared_ptr<CodeGenerator> getCodeGenerator(std::string codegen_name, std::shared_ptr<VirtualObjectFormat> object_format)
{
    std::shared_ptr<CodeGenerator> codegen = NULL;
    void* lib_addr = GoblinLoadLibrary(std::string(std::string(CODEGEN_DIR)
                                                   + "/" + codegen_name + std::string(CODEGEN_EXT)).c_str());

    if (lib_addr == NULL)
    {
        throw Exception("The code generator: " + codegen_name + " could not be found or loaded");
    }


    CodegenInitFunc init_func = (CodegenInitFunc) GoblinGetAddress(lib_addr, "Init");
    if (init_func == NULL)
    {
        throw Exception("The code generator: " + codegen_name + " does not have a valid \"Init\" function.");
    }

    // Call the libraries Init method and get their code generator.
    codegen = std::shared_ptr<CodeGenerator>(init_func(&compiler, object_format));

    if (codegen == NULL)
    {
        throw Exception("The code generator: " + codegen_name + " returned a NULL pointer when expecting a \"CodeGenerator\" object");
    }
    return codegen;
}

bool handle_parser_errors_and_warnings()
{
    std::shared_ptr<Logger> logger = parser->getLogger();
    std::vector<std::string> log = logger->getLog();
    for (std::string message : log)
    {
        std::cout << message << std::endl;
    }

    if (logger->hasAnError())
    {
        return false;
    }

    return true;

}

int main(int argc, char** argv)
{
    std::string codegen_name;
    std::string input_file_name;
    std::string output_file_name;
    std::string source_file_data;
    std::string obj_format_name;
    bool object_file_output = false;
    ArgumentContainer arguments;
    std::vector<std::string> file_names_to_link;

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
            arguments = GoblinArgumentParser_GetArguments(argc, argv);
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

            if (arguments.hasArgument("O"))
            {
                object_file_output = true;

                if (!arguments.hasArgument("format"))
                {
                    std::cout << "No object format defined, defaulting to cuof." << std::endl;
                    obj_format_name = "cuof";
                }
                else
                {
                    obj_format_name = arguments.getArgumentValue("format");
                }
            }

            if (arguments.hasArgument("L"))
            {
                if (object_file_output)
                {
                    std::cout << "You cannot link objects while outputting as an object file." << std::endl;
                    return PROBLEM_WITH_ARGUMENT;
                }
                else
                {
                    std::string to_link_str = arguments.getArgumentValue("L");
                    file_names_to_link = Helper::split(to_link_str, ',');
                }
            }

            input_file_name = arguments.getArgumentValue("input");
            output_file_name = arguments.getArgumentValue("output");

            if (input_file_name == output_file_name)
            {
                std::cout << "The input file and the output file may not be the same" << std::endl;
                return PROBLEM_WITH_ARGUMENT;
            }
        }
        catch (GoblinArgumentException ex)
        {
            std::cout << "Error parsing arguments: " + ex.getMessage() << std::endl;
            return ARGUMENT_PARSE_PROBLEM;
        }
    }

    // Let the compiler know the arguments that were passed to us.
    for(Argument argument : arguments.getArguments())
    {
        compiler.setArgument(argument.name, argument.value);
    }

    std::cout << "Compiling: " << input_file_name << " to " << output_file_name
            << (object_file_output ? " as an object file " : "") << ", code generator: " << codegen_name << std::endl;
    try
    {
        source_file_data = LoadFile(input_file_name);
    }
    catch (Exception ex)
    {
        std::cout << "Problem loading source file: " << ex.getMessage() << std::endl;
        return SOURCE_FILE_LOAD_FAILURE;
    }

    std::shared_ptr<VirtualObjectFormat> obj_format;

    try
    {
        obj_format = getObjectFormat(obj_format_name);
    }
    catch (Exception ex)
    {
        std::cout << "Problem loading object format: " << ex.getMessage() << std::endl;
        return OBJECT_FORMAT_LOAD_PROBLEM;
    }

    std::shared_ptr<CodeGenerator> codegen;
    try
    {
        codegen = getCodeGenerator(codegen_name, obj_format);
        compiler.setCodeGenerator(codegen);
    }
    catch (Exception ex)
    {
        std::cout << "Problem loading code generator: " << ex.getMessage() << std::endl;
        return CODEGENERATOR_LOAD_PROBLEM;
    }

    lexer = compiler.getLexer();
    parser = compiler.getParser();
    preprocessor = compiler.getPreprocessor();
    semanticValidator = compiler.getSemanticValidator();
    treeImprover = compiler.getTreeImprover();

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
        parser->setInput(lexer->getTokens());
        parser->buildTree();
    }
    catch (Exception ex)
    {
        std::cout << "Error parsing: " << ex.getMessage() << std::endl;
        handle_parser_errors_and_warnings();
        return ERROR_WITH_PARSER;
    }

    // Handle parsing warnings and errors
    handle_parser_errors_and_warnings();

    try
    {
        preprocessor->setTree(parser->getTree());
        preprocessor->process();
    }
    catch (Exception ex)
    {
        std::cout << "Error with preprocessor: " << ex.getMessage() << std::endl;
        return ERROR_WITH_PREPROCESSOR;
    }

    // Ensure the input is semantically correct
    try
    {
        semanticValidator->setTree(parser->getTree());
        semanticValidator->validate();
    }
    catch (SemanticValidatorException ex)
    {
        std::cout << "Error with validation: " << ex.getMessage() << std::endl;
        return ERROR_WITH_SEMANTIC_VALIDATION;
    }

    // Improve the tree
    // Get some exception handling here soon
    treeImprover->setTree(parser->getTree());
    treeImprover->improve();

#ifdef DEBUG_MODE
    debug_output_branch(parser->getTree()->root);
#endif 

    try
    {
        codegen->generate(parser->getTree());
        codegen->assemble();

        // Finalize the object
        std::shared_ptr<VirtualObjectFormat> obj_format = codegen->getObjectFormat();
        obj_format->finalize();

        // Ok lets write the object file
        if (object_file_output)
        {
            WriteFile(output_file_name, obj_format->getObjectStream());
        }
        /*
        // This is an object file output so there is no need for any linking
        if (object_file_output)
        {
            WriteFile(output_file_name, obj_stream);
        }
        else
        {
            try
            {
                linker->addObjectFileStream(obj_stream);
                for (std::string filename_to_link : file_names_to_link)
                {
                    std::cout << "Will link with: " << filename_to_link << std::endl;
                    linker->addObjectFile(filename_to_link);
                }
                linker->link();
                Stream* executable_stream = linker->getExecutableStream();
                WriteFile(output_file_name, executable_stream);
            }
            catch (Exception ex)
            {
                std::cout << ex.getMessage() << std::endl;
                return ERROR_WITH_OUTPUT_FILE;
            }
        }
         */
    }
    catch (Exception ex)
    {
        std::cout << "Error with code generator: " << ex.getMessage() << std::endl;
        return ERROR_WITH_CODEGENERATOR;
    }
    return 0;
}
