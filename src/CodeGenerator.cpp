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
#include "branches.h"

CodeGenerator::CodeGenerator(Compiler* compiler, std::string code_gen_desc) : CompilerEntity(compiler)
{
    this->stream = new Stream();
    this->current_index = 0;
    this->code_gen_desc = code_gen_desc;
    this->current_scope = NULL;
}

CodeGenerator::~CodeGenerator()
{
    delete this->stream;
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
            std::shared_ptr<struct variable> scope_var = this->current_scope->getVariable(value);
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
        this->startScope(branch);
        // Generate the scope
        // Register the function arguments as a scope variable
        for (std::shared_ptr<Branch> branch : func_argument_branches)
        {
            this->current_scope->registerVariableFromBranch(branch);
        }
        this->handleScope(scopeBranch);
        this->endScope(branch);

        this->registerFunction(funcBranch->getFunctionNameBranch()->getValue(), func_argument_branches, func_mem_pos);
    }
}

void CodeGenerator::startScope(std::shared_ptr<Branch> branch)
{
    std::shared_ptr<Scope> new_scope = Scope::newInstance(this->stream->getPosition());
    if (this->current_scope != NULL)
    {
        this->current_scope->setParentScope(new_scope);
    }
    this->current_scope = new_scope;
    this->scope_start(branch);
}

void CodeGenerator::endScope(std::shared_ptr<Branch> branch)
{
    if (this->current_scope == NULL)
    {
        throw CodeGeneratorException("void CodeGenerator::endScope(std::shared_ptr<Branch> branch): No scope to begin with.");
    }
    // Call scope_end just before the scope is removed.
    this->scope_end(branch);
    this->current_scope = this->current_scope->getParentScope();
}

std::shared_ptr<Scope> CodeGenerator::getCurrentScope()
{
    return this->current_scope;
}

void CodeGenerator::handleScope(std::shared_ptr<Branch> branch)
{
    bool handle_children = true;

    if (branch->getType() == "V_DEF")
    {
        this->current_scope->registerVariableFromBranch(branch);
    }
    else if (branch->getType() == "ASSIGN")
    {
        std::shared_ptr<AssignBranch> assign_branch = std::dynamic_pointer_cast<AssignBranch>(branch);
        std::string var_name = assign_branch->getVariableToAssignBranch()->getValue();
        std::shared_ptr<struct variable> var = this->current_scope->getVariable(var_name);
        this->scope_assignment(var, assign_branch, assign_branch->getValueBranch());
    }
    else if (branch->getType() == "CALL")
    {
        std::shared_ptr<CallBranch> callBranch = std::dynamic_pointer_cast<CallBranch>(branch);
        std::string name = callBranch->getFunctionNameBranch()->getValue();
        std::vector<std::shared_ptr < Branch>> arguments = callBranch->getFunctionArgumentBranches();
        for (std::shared_ptr<Branch> argument : arguments)
        {
            /*
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
             */
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