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
 * File:   CodeGenerator.cpp
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 19:49
 * 
 * Description: The base class for all code generators.
 * 
 * Code generators are objects that take in an AST(Abstract Syntax Tree) and convert it to some form of output
 * weather that be byte code, machine code or even source code.
 */

#include "CodeGenerator.h"
#include "CallBranch.h"
#include "FuncBranch.h"
#include "VDEFBranch.h"
#include "AssignBranch.h"
#include "MathEBranch.h"
#include "EBranch.h"
#include "ASMBranch.h"
#include "StructBranch.h"
#include "StructAssignBranch.h"
#include "SElementBranch.h"
#include "ArrayBranch.h"

CodeGenerator::CodeGenerator(Compiler* compiler, std::string code_gen_desc) : CompilerEntity(compiler)
{
    this->stream = new Stream();
    this->current_index = 0;
    this->code_gen_desc = code_gen_desc;
}

CodeGenerator::~CodeGenerator()
{
    delete this->stream;
}

scope_variable::scope_variable()
{

}

scope_variable::~scope_variable()
{

}

array_scope_variable::array_scope_variable()
{

}

array_scope_variable::~array_scope_variable()
{

}

struct_scope_variable::struct_scope_variable()
{

}

struct_scope_variable::~struct_scope_variable()
{

}

node::node()
{

}

node::~node()
{

}

bool structure::hasVariable(std::string name)
{
    return getVariable(name) != NULL;
}

std::shared_ptr<struct scope_variable> structure::getVariable(std::string name)
{
    for (int i = 0; i < this->variables.size(); i++)
    {
        std::shared_ptr<struct scope_variable> variable = this->variables[i];
        if (variable->name == name)
        {
            return variable;
        }
    }
    return NULL;
}

void CodeGenerator::registerFunction(std::string func_name, std::vector<std::shared_ptr<Branch>> func_arguments, int func_mem_pos)
{
    struct function func;
    func.func_name = func_name;
    for (std::shared_ptr<Branch> func_argument : func_arguments)
    {
        std::shared_ptr<VDEFBranch> vdef_func_argument = std::dynamic_pointer_cast<VDEFBranch>(func_argument);
        struct function_argument argument;
        argument.type = vdef_func_argument->getDefinitionTypeBranch()->getValue();
        argument.value = vdef_func_argument->getDefinitionNameBranch()->getValue();
        func.func_arguments.push_back(argument);
    }
    func.memory_pos = func_mem_pos;
    functions.push_back(func);
}

void CodeGenerator::registerFunctionCall(std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments)
{
    struct function_call func_call;
    struct function_argument func_call_argument;
    func_call.func_name = func_name;
    for (std::shared_ptr<Branch> func_argument_branch : func_arguments)
    {
        std::string value = func_argument_branch->getValue();
        std::string type = func_argument_branch->getType();
        if (type == "identifier")
        {
            std::shared_ptr<struct scope_variable> scope_var = this->getScopeVariable(value);
            func_call_argument.type = scope_var->type;
            func_call_argument.value = value;
        }
        else if (type == "number")
        {
            int value_num = std::atoi(value.c_str());
            std::string type = this->getCompiler()->getTypeFromNumber(value_num);
            func_call_argument.type = type;
            func_call_argument.value = value;
        }

        func_call.func_arguments.push_back(func_call_argument);
    }
    func_call.memory_pos = this->stream->getPosition();
    this->function_calls.push_back(func_call);
}

bool CodeGenerator::isFunctionRegistered(std::string func_name)
{
    for (int i = 0; i < this->functions.size(); i++)
    {
        struct function* func = &this->functions[i];
        if (func->func_name == func_name)
        {
            return true;
        }
    }
    return false;
}

int CodeGenerator::getFunctionIndex(std::string func_name)
{
    for (int i = 0; i < this->functions.size(); i++)
    {
        struct function* func = &this->functions[i];
        if (func->func_name == func_name)
        {
            return i;
        }
    }
    return -1;
}

void CodeGenerator::generate(std::shared_ptr<Tree> tree)
{
    std::shared_ptr<Branch> root = tree->root;
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        this->generateFromBranch(branch);
    }

    // Now its time to populate the goblin object file
    struct goblin_obj_header* gob_head = &this->gob_obj.header;
    struct goblin_functions_declaration_header* func_declarations_header = &gob_head->func_declarations_header;
    struct goblin_function_calls_header* func_calls_header = &gob_head->func_calls_header;

    size_t stream_size = this->stream->getSize();
    gob_head->code_generator_descriptor = this->code_gen_desc;

    int total_func_decl = this->functions.size();
    func_declarations_header->t_functions = total_func_decl;
    func_declarations_header->functions = new goblin_function_declaration[total_func_decl];
    if (total_func_decl > 0)
    {
        for (int i = 0; i < total_func_decl; i++)
        {
            struct function* function = &this->functions[i];
            int total_func_args = function->func_arguments.size();
            struct goblin_function_declaration* gob_func_decl = &func_declarations_header->functions[i];
            gob_func_decl->func_name = function->func_name;
            gob_func_decl->t_arguments = total_func_args;
            if (total_func_args > 0)
            {
                gob_func_decl->arguments = new goblin_argument[total_func_args];
            }
            for (int a = 0; a < total_func_args; a++)
            {
                struct function_argument* argument = &function->func_arguments[a];
                struct goblin_argument* gob_argument = &gob_func_decl->arguments[a];
                gob_argument->type = argument->type;
            }

            gob_func_decl->mem_pos = function->memory_pos;
        }
    }
    int total_func_calls = this->function_calls.size();
    func_calls_header->total_func_calls = total_func_calls;
    if (total_func_calls > 0)
    {
        func_calls_header->func_calls = new goblin_function_call[total_func_calls];


        for (int i = 0; i < total_func_calls; i++)
        {
            struct function_call* func_call = &this->function_calls[i];
            struct goblin_function_call* gob_func_call = &func_calls_header->func_calls[i];
            int total_arguments = func_call->func_arguments.size();
            gob_func_call->func_name = func_call->func_name;
            gob_func_call->t_arguments = func_call->func_arguments.size();
            if (total_arguments > 0)
            {
                gob_func_call->arguments = new goblin_argument[total_arguments];
                for (int b = 0; b < total_arguments; b++)
                {
                    struct function_argument* argument = &func_call->func_arguments[b];
                    struct goblin_argument* gob_argument = &gob_func_call->arguments[b];
                    gob_argument->type = argument->type;
                }
            }
            gob_func_call->mem_pos = func_call->memory_pos;
        }
    }

    gob_head->code_size = stream_size;
    Stream* code_stream = &gob_head->code;
    for (int i = 0; i < stream_size; i++)
    {
        code_stream->write8(this->stream->read8());
    }

    // Erase the stream as the code is now written in the code stream
    this->stream->empty();

    // Write the data to the goblin object stream
    gob_obj.write();

    // Store the goblin object in the stream
    Stream* gob_obj_stream = gob_obj.getStream();
    while (!gob_obj_stream->isEmpty())
    {
        this->stream->write8(gob_obj_stream->read8());
    }

}

void CodeGenerator::generateFromBranch(std::shared_ptr<Branch> branch)
{
    if (branch->getType() == "FUNC")
    {
        std::shared_ptr<FuncBranch> funcBranch = std::dynamic_pointer_cast<FuncBranch>(branch);
        std::vector<std::shared_ptr < Branch>> func_argument_branches = funcBranch->getFunctionArgumentBranches();
        std::shared_ptr<Branch> scopeBranch = funcBranch->getFunctionScopeBranches();
        int func_mem_pos = this->stream->getPosition();
        this->scope_start(branch);
        // Generate the scope
        // Register the function arguments as a scope variable
        for (std::shared_ptr<Branch> branch : func_argument_branches)
        {
            this->registerScopeVariable(branch);
        }
        this->handleScope(scopeBranch);
        this->scope_end(branch);

        this->clearScopeVariables();
        this->registerFunction(funcBranch->getFunctionNameBranch()->getValue(), func_argument_branches, func_mem_pos);
    }
    else if (branch->getType() == "STRUCT")
    {
        std::shared_ptr<StructBranch> struct_branch = std::dynamic_pointer_cast<StructBranch>(branch);
        this->registerStructure(struct_branch->getNameBranch()->getValue(), struct_branch->getScopeBranch());
        this->clearScopeVariables();
    }
}

void CodeGenerator::handleScope(std::shared_ptr<Branch> branch)
{
    bool handle_children = true;

    if (branch->getType() == "V_DEF")
    {
        this->registerScopeVariable(branch);
    }
    else if (branch->getType() == "ASSIGN")
    {
        std::shared_ptr<AssignBranch> assign_branch = std::dynamic_pointer_cast<AssignBranch>(branch);
        std::string var_name = assign_branch->getVariableToAssignBranch()->getValue();
        std::shared_ptr<struct scope_variable> var = this->getScopeVariable(var_name);
        this->scope_assignment(var, assign_branch, assign_branch->getValueBranch());
    }
    else if (branch->getType() == "CALL")
    {
        std::shared_ptr<CallBranch> callBranch = std::dynamic_pointer_cast<CallBranch>(branch);
        std::string name = callBranch->getFunctionNameBranch()->getValue();
        std::vector<std::shared_ptr < Branch>> arguments = callBranch->getFunctionArgumentBranches();
        for (std::shared_ptr<Branch> argument : arguments)
        {
            if (argument->getType() == "number")
            {
                this->scope_handle_number(argument);
            }
            else if (argument->getType() == "MATH_E")
            {
                std::vector<std::shared_ptr < Branch>> e_branches = this->getCompiler()->getASTAssistant()->findAllChildrenOfType(argument, "MATH_E");
                for (std::shared_ptr<Branch> e_branch : e_branches)
                {
                    std::shared_ptr<MathEBranch> math_e_branch = std::dynamic_pointer_cast<MathEBranch>(e_branch);
                    this->scope_handle_exp(math_e_branch);
                }

            }
        }

        this->registerFunctionCall(name, arguments);
        this->scope_func_call(branch, name, arguments);
    }
    else if (branch->getType() == "ASM")
    {
        this->scope_handle_inline_asm(branch);
    }


    if (handle_children)
    {
        std::vector<std::shared_ptr < Branch>> children = branch->getChildren();
        for (std::shared_ptr<Branch> child : children)
        {
            handleScope(child);
        }
    }

    handle_children = true;
}

int CodeGenerator::getScopeVariablesSize()
{
    int size = 0;
    for (std::shared_ptr<struct scope_variable> var : this->scope_variables)
    {
        size += var->size;
    }
    return size;
}

std::shared_ptr<struct scope_variable> CodeGenerator::registerScopeVariable(std::shared_ptr<Branch> branch)
{
    if (branch->getType() != "V_DEF")
    {
        throw CodeGeneratorException("The branch: " + branch->getType() + " cannot be converted to a scope variable");
    }


    std::shared_ptr<struct scope_variable> variable = this->createScopeVariable(branch);
    if (this->scope_variables.empty())
    {
        variable->mem_pos = 0;
    }
    else
    {
        std::shared_ptr<struct scope_variable> last_var = this->scope_variables.back();
        variable->mem_pos = last_var->mem_pos + last_var->size;
    }
    this->scope_variables.push_back(variable);

    return variable;
}

std::shared_ptr<struct scope_variable> CodeGenerator::createScopeVariable(std::shared_ptr<Branch> branch)
{
    std::shared_ptr<struct scope_variable> variable = NULL;
    std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
    std::shared_ptr<Token> name_branch = std::dynamic_pointer_cast<Token>(vdef_branch->getDefinitionNameBranch());
    std::shared_ptr<Token> type_branch = std::dynamic_pointer_cast<Token>(vdef_branch->getDefinitionTypeBranch());
    std::string type_value = type_branch->getValue();

    if (type_branch->getType() == "keyword")
    {
        int size_per_elem = this->getCompiler()->getDataTypeSize(type_value);
        if (vdef_branch->isArray())
        {
            std::shared_ptr<struct array_scope_variable> array_variable = std::shared_ptr<struct array_scope_variable > (new struct array_scope_variable());
            struct array_def arr = vdef_branch->getArray();
            array_variable->size = size_per_elem * arr.t_size;
            array_variable->size_per_element = size_per_elem;
            array_variable->is_array = true;
            variable = array_variable;
        }
        else
        {
            variable = std::shared_ptr<struct scope_variable > (new struct scope_variable);
            variable->size = size_per_elem;
            variable->is_array = false;
        }

        // Its a keyword so its a primitive type
        variable->custom = NOT_CUSTOM;

    }
    else
    {
        variable = std::shared_ptr<struct struct_scope_variable> (new struct struct_scope_variable);
        // It is not a keyword so it must be a custom type such as a struct ...
        struct structure* stru = this->getStructure(type_value);
        if (stru == NULL)
        {
            throw CodeGeneratorException(type_branch->getPosition(), "could not find structure \"" + type_value + "\" it is not declared");
        }

        variable->custom = CUSTOM_STRUCT;
        variable->size = stru->size;
    }

    variable->name = name_branch->getValue();
    variable->type = type_value;

    return variable;
}

std::shared_ptr<struct scope_variable> CodeGenerator::getScopeVariable(std::string name)
{
    for (std::shared_ptr<struct scope_variable> variable : this->scope_variables)
    {
        if (variable->name == name)
            return variable;
    }

    return NULL;
}

void CodeGenerator::clearScopeVariables()
{
    this->scope_variables.clear();
}

/* DEPRECATED */
int CodeGenerator::getRelativeIndexFromArrayBranch(std::shared_ptr<Branch> branch)
{
    int index = 0;
    std::shared_ptr<ArrayBranch> root_array_branch = std::dynamic_pointer_cast<ArrayBranch>(branch);
    if (root_array_branch->isBranchAnArrayHolder())
    {
        std::vector<std::shared_ptr < Branch>> children = root_array_branch->getChildren();
        std::shared_ptr<ArrayBranch> first_child = std::dynamic_pointer_cast<ArrayBranch>(children[0]);
        index = first_child->getIndexBranchNumber();
        for (int i = 1; i < children.size(); i++)
        {
            std::shared_ptr<ArrayBranch> child = std::dynamic_pointer_cast<ArrayBranch>(children[i]);
            index *= child->getIndexBranchNumber();
        }
    }
    else
    {
        index = root_array_branch->getIndexBranchNumber();
    }

    return index;
}

void CodeGenerator::registerStructure(std::string name, std::shared_ptr<Branch> scope)
{
    struct structure stru;
    std::vector<std::shared_ptr<struct scope_variable>> variables;
    std::vector<std::shared_ptr < Branch>> children = this->getCompiler()->getASTAssistant()->findAllChildrenOfType(scope, "V_DEF");
    stru.name = name;

    // This is still a part of the scope
    size_t t_size = 0;
    for (std::shared_ptr<Branch> branch : children)
    {
        std::shared_ptr<struct scope_variable> var = this->registerScopeVariable(branch);
        variables.push_back(var);
        t_size += var->size;
    }

    stru.size = t_size;

    stru.variables = variables;
    this->structures.push_back(stru);
}

void CodeGenerator::handleExp(std::shared_ptr<Branch> value_branch)
{
    std::vector<std::shared_ptr < Branch >> e_branches = this->getCompiler()->getASTAssistant()->findAllChildrenOfType(value_branch, "MATH_E");
    if (e_branches.size() != 0)
    {
        this->scope_exp_start();
    }
    for (std::shared_ptr<Branch> e_branch : e_branches)
    {
        std::shared_ptr<MathEBranch> math_e_branch = std::dynamic_pointer_cast<MathEBranch>(e_branch);
        this->scope_handle_exp(math_e_branch);
    }
    if (e_branches.size() != 0)
    {
        this->scope_exp_end();
    }
}

std::shared_ptr<struct node> CodeGenerator::handleObject(std::shared_ptr<Branch> branch)
{
    std::shared_ptr<struct node> root_node;
    std::string branch_type = branch->getType();
    if (branch_type == "number")
    {
        // Handle the number
        int number = std::stoi(branch->getValue(), NULL);
        std::shared_ptr<struct direct_integer_node> node = std::shared_ptr<direct_integer_node>(new struct direct_integer_node);
        root_node->type = TYPE_INTEGER_DIRECT;
        node->value = number;
        root_node = node;
    }
    else if (branch_type == "identifier")
    {
        // Handle the identifier
        this->scope_handle_identifier(branch);
    }
    else if (branch_type == "E")
    {
        // Handle the array
        std::shared_ptr<Branch> array_name_branch = branch->getChildren()[0]->getChildren()[0];
        std::shared_ptr<Branch> array_root_index_branch = branch->getChildren()[1];
   //     this->scope_handle_array_access(array_name_branch->getValue(), array_root_index_branch);
    }
    else
    {
        goto error;
    }

    return root_node;


error:
    throw CodeGeneratorException("CodeGenerator::handleObject(std::shared_ptr<Branch> branch): cannot handle branch: " + branch_type);
}

struct structure* CodeGenerator::getStructure(std::string name)
{
    for (int i = 0; i < this->structures.size(); i++)
    {
        struct structure* stru = &this->structures[i];
        if (stru->name == name)
        {
            return stru;
        }
    }

    return NULL;
}

int CodeGenerator::getMemoryLocationForArrayAccess(std::string array_var_name, std::shared_ptr<Branch> array_index_root_branch)
{
    std::shared_ptr<struct array_scope_variable> array_scope_var = std::dynamic_pointer_cast<struct array_scope_variable>(this->getScopeVariable(array_var_name));
    std::vector<std::shared_ptr < Branch>> root_index_branch_children = array_index_root_branch->getChildren();
    if (array_scope_var == NULL)
    {
        throw CodeGeneratorException("CodeGenerator::getMemoryLocationForArrayAccess(std::string array_var_name, std::shared_ptr<Branch> array_index_root_branch): array variable not found.");
    }
    int mem_pos = array_scope_var->mem_pos;

    // Only supports one dimension at the moment.
    if (root_index_branch_children.size() != 1)
    {
        throw CodeGeneratorException("CodeGenerator::getMemoryLocationForArrayAccess(std::string array_var_name, std::shared_ptr<Branch> array_index_root_branch): Only one index is supported at the moment");
    }

    std::shared_ptr<Branch> index_number_branch = root_index_branch_children[0]->getChildren()[0];
    if (index_number_branch->getType() != "number")
    {
        throw CodeGeneratorException("CodeGenerator::getMemoryLocationForArrayAccess(std::string array_var_name, std::shared_ptr<Branch> array_index_root_branch): only numbers can be used as indexes at the moment");
    }

    mem_pos += std::stoi(index_number_branch->getValue()) * array_scope_var->size_per_element;
    return mem_pos;
}

GoblinObject* CodeGenerator::getGoblinObject()
{
    return &this->gob_obj;
}

std::string CodeGenerator::getCodeGeneratorDescriptor()
{
    return this->code_gen_desc;
}

Stream* CodeGenerator::getStream()
{
    return this->stream;
}