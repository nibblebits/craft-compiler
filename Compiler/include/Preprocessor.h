/*
    Craft compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   Preprocessor.h
 * Author: Daniel McCarthy
 *
 * Created on 08 January 2017, 01:58
 */

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <memory>
#include <map>
#include <string>
#include "CompilerEntity.h"
#include "Logger.h"
class Tree;
class Branch;
class MacroIfDefBranch;
class MacroDefineBranch;
class MacroDefinitionIdentifierBranch;
class MacroFuncCallBranch;
class BODYBranch;
class FuncBranch;

enum
{
    PREPROCESSOR_DEFINITION_TYPE_UNKNOWN,
    PREPROCESSOR_DEFINITION_TYPE_NUMBER,
    PREPROCESSOR_DEFINITION_TYPE_STRING
};

enum
{
    MACRO_FUNCTION_ARGUMENTS_NO_LIMIT = -1
};

typedef int PREPROCESSOR_DEF_TYPE;

struct preprocessor_def
{
    std::string name;
    std::string value;
    PREPROCESSOR_DEF_TYPE type;
};

struct macro_function
{
    std::string func_name;
    int max_args;
    std::function<int(std::shared_ptr<Branch> args) > function;
};

class EXPORT Preprocessor : public CompilerEntity
{
public:
    Preprocessor(Compiler* compiler);
    virtual ~Preprocessor();
    void setTree(std::shared_ptr<Tree> tree);
    void register_macro_function(std::string function_name, int max_args, std::function<int(std::shared_ptr<Branch> args) > function);
    int invoke_macro_function(std::string function_name, std::shared_ptr<Branch> args);
    struct macro_function get_macro_function(std::string function_name);
    bool is_macro_function_registered(std::string function_name);
    void process();
    bool is_macro(std::string macro_name);
    bool is_definition_registered(std::string definition_name);
    void define_definition(std::string definition_name, std::string value, PREPROCESSOR_DEF_TYPE def_type);
    std::string get_definition_value(std::string definition_name);
    struct preprocessor_def get_definition(std::string definition_name);
    std::shared_ptr<Logger> getLogger();
private:
    void process_macro(std::shared_ptr<Branch> macro);
    void process_macro_ifdef(std::shared_ptr<MacroIfDefBranch> macro_ifdef_branch);
    void process_macro_define(std::shared_ptr<MacroDefineBranch> macro_define_branch);
    void process_macro_def_identifier(std::shared_ptr<MacroDefinitionIdentifierBranch> macro_def_iden_branch);
    void process_macro_func_call(std::shared_ptr<MacroFuncCallBranch> macro_func_call_branch);

    void process_expression(std::shared_ptr<Branch> child);
    void process_expression_part(std::shared_ptr<Branch> child);
    void process_child(std::shared_ptr<Branch> child);
    void process_func(std::shared_ptr<FuncBranch> func_branch);
    void process_body(std::shared_ptr<BODYBranch> body_branch);
    std::string evaluate_expression(std::shared_ptr<Branch> value_branch, PREPROCESSOR_DEF_TYPE* def_type_ptr);
    std::string evaluate_expression_part(std::shared_ptr<Branch> value_branch, PREPROCESSOR_DEF_TYPE* def_type_ptr);
    bool is_string_numeric_only(std::string str);
    std::shared_ptr<Tree> tree;
    std::map<std::string, struct preprocessor_def> definitions;
    std::map<std::string, struct macro_function> macro_functions;
    std::shared_ptr<Logger> logger;
};

#endif /* PREPROCESSOR_H */

