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
#include "Linker.h"
#include "CompilerEntity.h"

enum
{
    NOT_CUSTOM,
    CUSTOM_STRUCT
};

enum
{
    TYPE_INTEGER_DIRECT = 0
};
struct scope_variable
{
    scope_variable();
    virtual ~scope_variable();
    std::string type;
    std::string name;
    int custom;
    int size;
    int mem_pos;
    bool is_array;

};

struct array_scope_variable : public scope_variable
{
    array_scope_variable();
    virtual ~array_scope_variable();
    int size_per_element;
};

struct struct_scope_variable : public scope_variable
{
    struct_scope_variable();
    virtual ~struct_scope_variable();

};

struct structure
{
    std::string name;
    int size;
    std::vector<std::shared_ptr<struct scope_variable>> variables;

    bool hasVariable(std::string name);
    std::shared_ptr<struct scope_variable> getVariable(std::string name);
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

struct node
{
    int type;
    node();
    virtual ~node() = 0;
};

struct direct_integer_node : public node
{
    int value;

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
    std::shared_ptr<struct scope_variable> registerScopeVariable(std::shared_ptr<Branch> branch);
    std::shared_ptr<struct scope_variable> createScopeVariable(std::shared_ptr<Branch> branch);
    std::shared_ptr<struct scope_variable> getScopeVariable(std::string name);
    int getScopeVariablesSize();
    void clearScopeVariables();
    int getRelativeIndexFromArrayBranch(std::shared_ptr<Branch> branch);
    

    void registerStructure(std::string name, std::shared_ptr<Branch> scope);
    struct structure* getStructure(std::string name);

    GoblinObject* getGoblinObject();
    std::string getCodeGeneratorDescriptor();

    virtual void generate(std::shared_ptr<Tree> tree);
    virtual void generateFromBranch(std::shared_ptr<Branch> branch);
    virtual void handleScope(std::shared_ptr<Branch> branch);

    virtual std::shared_ptr<Linker> getLinker() = 0;
    virtual void scope_start(std::shared_ptr<Branch> branch) = 0;
    virtual void scope_assignment(std::shared_ptr<struct scope_variable> var, std::shared_ptr<Branch> assign_root, std::shared_ptr<Branch> assign_to) = 0;
    virtual void scope_func_call(std::shared_ptr<Branch> branch, std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments) = 0;
    virtual void scope_end(std::shared_ptr<Branch> branch) = 0;
    virtual void scope_exp_start() = 0;
    virtual void scope_handle_exp(std::shared_ptr<Branch> branch) = 0;
    virtual void scope_exp_end() = 0;
    virtual void scope_handle_number(std::shared_ptr<Branch> branch) = 0;
    virtual void scope_handle_identifier(std::shared_ptr<Branch> branch) = 0;
    virtual void scope_handle_inline_asm(std::shared_ptr<Branch> branch) = 0;
    virtual void scope_struct_assign_start(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> struct_ins_var, std::shared_ptr<struct scope_variable> struct_attr_var) = 0;
    virtual void scope_struct_assign_end(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> struct_ins_var, std::shared_ptr<struct scope_variable> struct_attr_var) = 0;
protected:
    Stream* stream;
    
    int getMemoryLocationForArrayAccess(std::string array_var_name, std::shared_ptr<Branch> array_index_root_branch);
private:
    void handleExp(std::shared_ptr<Branch> value_branch);
    std::shared_ptr<struct node> handleObject(std::shared_ptr<Branch> branch);
    std::string code_gen_desc;
    int current_index;
    std::vector<struct function> functions;
    std::vector<struct function_call> function_calls;
    std::vector<struct structure> structures;
    std::vector<std::shared_ptr<struct scope_variable>> scope_variables;
    GoblinObject gob_obj;
};

#endif /* CODEGENERATOR_H */

