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
typedef Linker* (*LinkerInitFunc)(Compiler*);

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
    ERROR_WITH_OBJECT_FORMAT = 14,
    ERROR_WITH_LINKER = 15
} CompilerErrorCode;

Compiler compiler;

Lexer* lexer;
Parser* parser;
SemanticValidator* semanticValidator;
TreeImprover* treeImprover;
Preprocessor* preprocessor;

std::string codegen_name;
std::string input_file_name;
std::string output_file_name;
std::string source_file_data;
std::string obj_format_name;
std::string exe_format;
bool object_file_output = false;

ArgumentContainer arguments;

std::shared_ptr<Linker> getLinker(std::string linker_name)
{
    std::shared_ptr<Linker> linker = NULL;
    void* lib_addr = GoblinLoadLibrary(std::string(std::string(LINKER_DIR)
                                                   + "/" + linker_name + std::string(LIBRARY_EXT)).c_str());
    if (lib_addr == NULL)
    {
        throw Exception("The linker: " + linker_name + " could not be found or loaded");
    }

    LinkerInitFunc init_func = (LinkerInitFunc) GoblinGetAddress(lib_addr, "Init");
    if (init_func == NULL)
    {
        throw Exception("The linker: " + linker_name + " does not have a valid \"Init\" function");
    }

    linker = std::shared_ptr<Linker>(init_func(&compiler));

    if (linker == NULL)
    {
        throw Exception("The linker: " + linker_name + " returned a NULL pointer when expecting a \"Linker\" object");
    }

    return linker;
}

VirtualObjectFormat* getObjectFormat(std::string object_format_name)
{
    VirtualObjectFormat* virtual_obj_format = NULL;
    void* lib_addr = GoblinLoadLibrary(std::string(std::string(OBJ_FORMAT_DIR)
                                                   + "/" + object_format_name + std::string(LIBRARY_EXT)).c_str());
    if (lib_addr == NULL)
    {
        throw Exception("The object format: " + object_format_name + " could not be found or loaded");
    }

    VirtualObjFormatInitFunc init_func = (VirtualObjFormatInitFunc) GoblinGetAddress(lib_addr, "Init");
    if (init_func == NULL)
    {
        throw Exception("The virtual object format: " + object_format_name + " does not have a valid \"Init\" function");
    }

    virtual_obj_format = init_func(&compiler);

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
                                                   + "/" + codegen_name + std::string(LIBRARY_EXT)).c_str());

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

std::string getFileExtension(std::string filename)
{
    std::vector<std::string> split = Helper::split(filename, '.');
    if (split.size() < 2)
    {
        throw Exception("File has no extension", "std::string getFileExtension(std::string filename)");
    }

    return split.at(split.size() - 1);
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

bool handle_preprocessor_errors_and_warnings()
{
    std::shared_ptr<Logger> logger = preprocessor->getLogger();
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

bool handle_semantic_validation_errors_and_warnings()
{
    std::shared_ptr<Logger> logger = semanticValidator->getLogger();
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

/*
 * Generates an object file based on source file input
 */
int GenerateMode()
{
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

    if (!arguments.hasArgument("format"))
    {
        std::cout << "No object format defined, defaulting to omf." << std::endl;
        obj_format_name = "omf";
    }
    else
    {
        obj_format_name = arguments.getArgumentValue("format");
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

    if (arguments.hasArgument("L"))
    {
        if (object_file_output)
        {
            std::cout << "You cannot link objects while outputting as an object file." << std::endl;
            return PROBLEM_WITH_ARGUMENT;
        }
    }

    input_file_name = arguments.getArgumentValue("input");
    output_file_name = arguments.getArgumentValue("output");

    if (input_file_name == output_file_name)
    {
        std::cout << "The input file and the output file may not be the same" << std::endl;
        return PROBLEM_WITH_ARGUMENT;
    }

    // We now need to compile the input into an object file

    std::cout << "Compiling: " << input_file_name << " to " << output_file_name
            << " code generator: " + codegen_name << std::endl;

    try
    {
        std::shared_ptr<Stream> input_stream = LoadFile(input_file_name);
        source_file_data = std::string(input_stream->getBuf(), input_stream->getSize());
    }
    catch (Exception& ex)
    {
        std::cout << "Problem loading source file: " << ex.getMessage() << std::endl;
        return SOURCE_FILE_LOAD_FAILURE;
    }

    std::shared_ptr<VirtualObjectFormat> obj_format;

    try
    {
        obj_format = std::shared_ptr<VirtualObjectFormat>(getObjectFormat(obj_format_name));
        obj_format->setFileName(output_file_name);
    }
    catch (Exception& ex)
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
    catch (Exception& ex)
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
    catch (Exception ex)
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

    // handle any errors relating to branches.
    if (!handle_preprocessor_errors_and_warnings())
    {
        return ERROR_WITH_PREPROCESSOR;
    }

#ifdef DEBUG_MODE
    debug_output_branch(parser->getTree()->root);
#endif 

    // Improve the tree
    try
    {
        treeImprover->setTree(parser->getTree());
        treeImprover->improve();
    }
    catch (Exception ex)
    {
        std::cout << "Error with improving the tree: " << ex.getMessage() << std::endl;
        return ERROR_WITH_TREE_IMPROVER;
    }

    // Ensure the input is semantically correct
    try
    {
        semanticValidator->setTree(parser->getTree());
        semanticValidator->validate();
    }
    catch (Exception ex)
    {
        std::cout << "Error with validation: " << ex.getMessage() << std::endl;
        handle_semantic_validation_errors_and_warnings();
        return ERROR_WITH_SEMANTIC_VALIDATION;
    }

    // Handle any errors and warnings relating to the input
    if (!handle_semantic_validation_errors_and_warnings())
    {
        return ERROR_WITH_SEMANTIC_VALIDATION;
    }

#ifdef DEBUG_MODE
    debug_output_branch(parser->getTree()->root);
#endif 
    try
    {
        codegen->generate(parser->getTree());
        codegen->assemble();

        // Finalize the object
        std::shared_ptr<VirtualObjectFormat> obj_format = std::shared_ptr<VirtualObjectFormat>(codegen->getObjectFormat());
        obj_format->finalize();

        // Ok lets write the object file
        WriteFile(output_file_name, obj_format->getObjectStream());

    }
    catch (Exception& ex)
    {
        std::cout << "Error with code generator: " << ex.getMessage() << std::endl;
        return ERROR_WITH_CODEGENERATOR;
    }

    return 0;
}

/* Links object files to form an executable */
int LinkMode()
{
    std::vector<std::string> file_names_to_link;
    std::vector<std::shared_ptr < VirtualObjectFormat>> obj_files;

    if (!arguments.hasArgument("input"))
    {
        std::cout << "You must provide multiple input files to link with, use -input \"obj1,obj2\"" << std::endl;
        return PROBLEM_WITH_ARGUMENT;
    }

    if (!arguments.hasArgument("output"))
    {
        std::cout << "You must provide an output file, use -output filename" << std::endl;
        return PROBLEM_WITH_ARGUMENT;
    }

    if (arguments.hasArgument("format"))
    {
        exe_format = arguments.getArgumentValue("format");
    }
    else
    {
        std::cout << "No executable format defined, defaulting to bin" << std::endl;
        exe_format = "bin";
    }


    output_file_name = arguments.getArgumentValue("output");
    file_names_to_link = Helper::split(arguments.getArgumentValue("input"), ',');


    std::cout << "Will link files: " << arguments.getArgumentValue("input") <<
            " to produce executable of type \"" << arguments.getArgumentValue("format") <<
            "\" at location \"" << arguments.getArgumentValue("output") << "\"" << std::endl;

    // We must load the executable format linker
    std::shared_ptr<Linker> linker;
    try
    {
        linker = getLinker(exe_format);
        compiler.setLinker(linker);
    }
    catch (Exception ex)
    {
        std::cout << "Error loading linker: " + ex.getMessage() << std::endl;
        return ERROR_WITH_LINKER;
    }

    // We must load the object files into memory, we also need to take their type into consideration
    for (std::string file_name : file_names_to_link)
    {
        std::cout << "Loading " << file_name << std::endl;
        std::string file_ext = getFileExtension(file_name);
        try
        {
            std::shared_ptr<VirtualObjectFormat> obj_format = std::shared_ptr<VirtualObjectFormat>(getObjectFormat(file_ext));
            obj_format->setFileName(file_name);
            std::vector<std::string> include_vec = compiler.getIncludeDirs();
            // Push the stdlib to the end of the include vector
            include_vec.push_back(compiler.getStdLibAddress());
            std::shared_ptr<Stream> stream = LoadFile(file_name, include_vec);
            try
            {
                obj_format->read(stream);
            }
            catch (Exception ex)
            {
                std::cout << "There was a problem reading from or translating the object stream for object format "
                        "of type \"" << file_ext << "\"" << ", for input file: \"" << file_name << "\"." <<
                        " Detailed message: " << ex.getMessage() << std::endl;
                return ERROR_WITH_OBJECT_FORMAT;
            }

#ifdef DEBUG_MODE
            // Lets output debugging information as debug mode is enabled
            debug_virtual_object_format(obj_format);
#endif

            // Ok we have everything we need let's add the object file to a vector for later processing
            obj_files.push_back(obj_format);
        }
        catch (Exception ex)
        {
            std::cout << "Could not load object format of type \"" <<
                    file_ext << "\" for input file: \"" << file_name << "\"." <<
                    " Please make sure a virtual object format exists for the given type." << std::endl <<
                    "Detailed reason for failure: \"" << ex.getMessage() << "\"" << std::endl;
            return ERROR_WITH_OBJECT_FORMAT;
        }
    }



    // Now link the files together
    for (std::shared_ptr<VirtualObjectFormat> obj_file : obj_files)
    {
        linker->addObjectFile(obj_file);
    }

    try
    {
        linker->link();
        std::cout << "Link successful" << std::endl;
    }
    catch (Exception ex)
    {
        std::cout << "Failed to link: " + ex.getMessage() << std::endl;
    }

    // Ok lets write the executable file
    try
    {
        WriteFile(output_file_name, linker->getExecutableStream());
    }
    catch (Exception ex)
    {
        std::cout << "Failed to output executable file" << std::endl;
        return ERROR_WITH_OUTPUT_FILE;
    }

    return 0;
}

void HelpMenu()
{
    std::cout << "HELP MENU" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << "For generating" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "First specify -O to state you wish to produce an object file" << std::endl;
    std::cout << "To specify an input file: -input \"filename\"" << std::endl;
    std::cout << "To specify an output file: -output \"filename\"" << std::endl;
    std::cout << "To specify a code generator: -codegen \"codegen_name\" e.g -codegen \"8086CodeGen\"" << std::endl;
    std::cout << "To specify an object file to output: -format \"object_format_name\" e.g -format \"omf\"" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Full Example: craft -input \"test_file.craft\" -output \"test.omf\" -codegen \"8086CodeGen\" -O -format \"omf\"" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << std::endl;
    std::cout << "For linking" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "First specify -L to state you wish to link" << std::endl;
    std::cout << "To specify input files: -input \"obj1.ext,obj2.ext,obj3.ext\" e.g -input \"file.omf,file2.omf\"" << std::endl;
    std::cout << "To specify an output file: -output \"filename\"" << std::endl;
    std::cout << "To specify a format to link to: -format \"type\" e.g -format \"bin\" for binary files" << std::endl;
    std::cout << "To specify origins for particular segments use: -org_segment_name \"origin\" e.g -org_data \"0x100\" for the data segment to origin at 0x100" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Full Example: craft -input \"test.omf,test2.omf,test3.omf\" -output \"test.com\" -L -format \"bin\" -org_data \"0x100\"" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Also ensure that you do not use the equal sign while using these options, e.g -input = \"name\" is illegal use -input \"name\"" << std::endl;
}

#include "InvokeableReturnHook.h"

int main(int argc, char** argv)
{
    arguments = GoblinArgumentParser_GetArguments(argc, argv);

    std::cout << COMPILER_FULLNAME << std::endl;
#ifdef DEBUG_MODE
    std::cout << "This is a debugging compiler; Build \"make release\" if this is a mistake" << std::endl;
#endif
    if (argc == 1)
    {
        std::cout << "No arguments provided, provide the argument \"-help\" for more information" << std::endl;
        return NO_ARGUMENTS_PROVIDED;
    }

    // Let the compiler know the arguments that were passed to us.
    for (Argument argument : arguments.getArguments())
    {
        compiler.setArgument(argument.name, argument.value);
    }

    // Let the compiler know about the stdlib
    if (arguments.hasArgument("stdlib"))
    {
        compiler.setStdLib(arguments.getArgumentValue("stdlib"));
    }

    // Let the compiler know about the include directories
    if (arguments.hasArgument("I"))
    {
        compiler.setIncludeDirs(Helper::split(arguments.getArgumentValue("I"), ','));
    }

    if (arguments.hasArgument("help"))
    {
        HelpMenu();
        return 0;
    }

    // Some error checking
    if (arguments.hasArgument("O") && arguments.hasArgument("L"))
    {
        std::cout << "You have specified to produce an output file and also link. This is not yet supported please choose one or the other." << std::endl;
        return PROBLEM_WITH_ARGUMENT;
    }
    else if (arguments.hasArgument("L"))
    {
        if (arguments.hasArgument("codegen"))
        {
            std::cout << "Code generator: " << arguments.getArgumentValue("codegen") << " has been provided but this is pointless as we are linking not generating. Please remove -codegen" << std::endl;
            return PROBLEM_WITH_ARGUMENT;
        }
    }

    // We need to find out if we are linking or generating
    if (arguments.hasArgument("O"))
    {
        // Ok we are compiling to produce an object file
        return GenerateMode();
    }
    else if (arguments.hasArgument("L"))
    {
        // Ok we are linking object files together to produce an executable file
        return LinkMode();
    }
    else
    {
        std::cout << "I do not know weather to produce an object file or link to create an executable, please specify either -O or -L" << std::endl;
        return PROBLEM_WITH_ARGUMENT;
    }

    return 0;
}
