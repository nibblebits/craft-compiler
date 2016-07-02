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
 * File:   CodeGenerator.h
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 19:49
 */

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
#include <vector>
#include <string>
#include <map>
#include "Tree.h"
#include "Stream.h"
#include "Branch.h"
#include "CodeGeneratorException.h"
#include "GoblinObject.h"
#include "CompilerEntity.h"


struct scope_variable
{
    std::string type;
    std::string name;
    int size;
    int index;
};

struct function
{
    std::string func_name;
    std::vector<struct function_argument> func_arguments;
    int memory_pos;
};

struct function_argument
{
    std::string type;
    std::string value;
};

struct function_call
{
    std::string func_name;
    std::vector<struct function_argument> func_arguments;
    int memory_pos;
};

class CodeGenerator : public CompilerEntity
{
public:
    CodeGenerator(Compiler* compiler, std::string code_gen_desc);
    virtual ~CodeGenerator();
    Stream* getStream();

    void registerFunction(std::string func_name, std::vector<std::shared_ptr<Branch>> func_arguments, int func_mem_pos);
    void registerFunctionCall(std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments);
    bool isFunctionRegistered(std::string func_name);
    int getFunctionIndex(std::string func_name);
    void registerScopeVariable(std::shared_ptr<Branch> branch);
    std::shared_ptr<struct scope_variable> getScopeVariable(std::string name);
    int getScopeVariablesSize();
    void clearScopeVariables();
    
    GoblinObject* getGoblinObject();
    std::string getCodeGeneratorDescriptor();
    
    virtual void generate(std::shared_ptr<Tree> tree);
    virtual void generateFromBranch(std::shared_ptr<Branch> branch);
    virtual void handleScope(std::shared_ptr<Branch> branch);

    virtual void scope_start(std::shared_ptr<Branch> branch) = 0;
    virtual void scope_assign_start(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable>) = 0;
    virtual void scope_assign_end(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable>) = 0;
    virtual void scope_func_call(std::shared_ptr<Branch> branch, std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments) = 0;
    virtual void scope_end(std::shared_ptr<Branch> branch) = 0;
    virtual void scope_handle_exp(std::shared_ptr<Branch> branch) = 0;
    virtual void scope_handle_number(std::shared_ptr<Branch> branch) = 0;
protected:
    Stream* stream;
private:
    std::string code_gen_desc;
    int current_index;
    std::vector<struct function> functions;
    std::vector<struct function_call> function_calls;
    std::vector<std::shared_ptr<struct scope_variable>> scope_variables;
    GoblinObject gob_obj;
};

#endif /* CODEGENERATOR_H */

