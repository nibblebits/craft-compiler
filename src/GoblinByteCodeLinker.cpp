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
 * File:   GoblinByteCodeLinker.cpp
 * Author: Daniel McCarthy
 *
 * Created on 03 July 2016, 01:53
 * 
 * Description: The linker for linking Goblin objects whose code generator is "goblin_codegen". These object files
 * contain byte code for the Goblin virtual machine.
 */

#include "GoblinByteCodeLinker.h"
#include "GoblinByteCodeGenerator.h"
#include "Compiler.h"

GoblinByteCodeLinker::GoblinByteCodeLinker(Compiler* compiler) : GoblinObjectLinker(compiler)
{
}

GoblinByteCodeLinker::~GoblinByteCodeLinker()
{
}

void GoblinByteCodeLinker::link_merge_part(GoblinObject* obj, GoblinObject* result_obj, GoblinObject* prev_obj)
{
    struct goblin_obj_header* obj_head = &obj->header;
    struct goblin_obj_header* result_obj_head = &result_obj->header;
    struct goblin_functions_declaration_header* obj_func_decl_head = &obj_head->func_declarations_header;
    struct goblin_functions_declaration_header* result_func_decl_head = &result_obj_head->func_declarations_header;
    struct goblin_function_calls_header* obj_func_calls_head = &obj_head->func_calls_header;
    struct goblin_function_calls_header* result_func_calls_head = &result_obj_head->func_calls_header;

    int code_offset = (prev_obj != NULL) ? prev_obj->header.code_size : 0;
    int func_result_start_index = (prev_obj != NULL) ? prev_obj->header.func_declarations_header.t_functions : 0;
    int func_call_result_start_index = (prev_obj != NULL) ? prev_obj->header.func_calls_header.total_func_calls : 0;

    for (int i = 0; i < obj_func_decl_head->t_functions; i++)
    {
        struct goblin_function_declaration* obj_func_decl = &obj_func_decl_head->functions[i];
        struct goblin_function_declaration* result_func_decl = &result_func_decl_head->functions[func_result_start_index + i];
        result_func_decl->func_name = obj_func_decl->func_name;
        result_func_decl->t_arguments = obj_func_decl->t_arguments;
        if (result_func_decl->t_arguments != 0)
        {
            result_func_decl->arguments = new goblin_argument[result_func_decl->t_arguments];
            for (int a = 0; a < result_func_decl->t_arguments; a++)
            {
                struct goblin_argument* result_func_argument = &result_func_decl->arguments[a];
                result_func_argument->type = obj_func_decl->arguments[a].type;
            }
        }
        result_func_decl->mem_pos = obj_func_decl->mem_pos + code_offset;
    }

    for (int i = 0; i < obj_func_calls_head->total_func_calls; i++)
    {
        // Copy the function call from the object into the resulting object
        struct goblin_function_call* gob_obj_func_call = &obj_func_calls_head->func_calls[i];
        struct goblin_function_call* gob_result_func_call = &result_func_calls_head->func_calls[func_call_result_start_index + i];
        gob_result_func_call->func_name = gob_obj_func_call->func_name;
        gob_result_func_call->t_arguments = gob_obj_func_call->t_arguments;
        if (gob_result_func_call->t_arguments != 0)
        {
            gob_result_func_call->arguments = new goblin_argument[gob_result_func_call->t_arguments];
            for (int a = 0; a < gob_result_func_call->t_arguments; a++)
            {
                struct goblin_argument* result_func_call_argument = &gob_result_func_call->arguments[a];
                result_func_call_argument->type = gob_obj_func_call->arguments[a].type;
            }
        }
        gob_result_func_call->mem_pos = gob_obj_func_call->mem_pos + code_offset;
    }

}

void GoblinByteCodeLinker::link_merge(GoblinObject* obj1, GoblinObject* obj2, GoblinObject* result_obj)
{
    struct goblin_obj_header* obj1_head = &obj1->header;
    struct goblin_obj_header* obj2_head = &obj2->header;
    struct goblin_obj_header* result_obj_head = &result_obj->header;
    struct goblin_functions_declaration_header* obj1_func_decl_head = &obj1_head->func_declarations_header;
    struct goblin_functions_declaration_header* obj2_func_decl_head = &obj2_head->func_declarations_header;
    struct goblin_functions_declaration_header* result_func_decl_head = &result_obj_head->func_declarations_header;
    struct goblin_function_calls_header* obj1_func_calls_head = &obj1_head->func_calls_header;
    struct goblin_function_calls_header* obj2_func_calls_head = &obj2_head->func_calls_header;
    struct goblin_function_calls_header* result_func_calls_head = &result_obj_head->func_calls_header;

    // Check these objects are valid
    if (obj1_head->code_generator_descriptor != obj2_head->code_generator_descriptor)
    {
        throw LinkerException("These objects do not share the same target code!");
    }

    result_obj_head->code_generator_descriptor = obj1_head->code_generator_descriptor;
    // Must be pre-calculated before calling link_merge_do
    result_func_decl_head->t_functions = obj1_func_decl_head->t_functions + obj2_func_decl_head->t_functions;
    if (result_func_decl_head->t_functions != 0)
    {
        result_func_decl_head->functions = new goblin_function_declaration[result_func_decl_head->t_functions];
    }
    result_func_calls_head->total_func_calls = obj1_func_calls_head->total_func_calls + obj2_func_calls_head->total_func_calls;
    if (result_func_calls_head->total_func_calls != 0)
    {
        result_func_calls_head->func_calls = new goblin_function_call[result_func_calls_head->total_func_calls];
    }
    result_obj_head->code_size = obj1_head->code_size + obj2_head->code_size;

    // Merge them to the result
    link_merge_part(obj1, result_obj);
    link_merge_part(obj2, result_obj, obj1);

    // Write the code Streams of both objects to the result object
    while (!obj1_head->code.isEmpty())
    {
        result_obj_head->code.write8(obj1_head->code.read8());
    }


    while (!obj2_head->code.isEmpty())
    {
        result_obj_head->code.write8(obj2_head->code.read8());
    }

    result_obj->write();
}

void GoblinByteCodeLinker::final_merge(Stream* executable_stream, GoblinObject* final_obj)
{
    struct goblin_obj_header* obj_head = &final_obj->header;
    struct goblin_function_calls_header* gob_func_calls_head = &obj_head->func_calls_header;
    struct goblin_functions_declaration_header* gob_funcs_declaration_head = &obj_head->func_declarations_header;
    Stream* code = &obj_head->code;
    int total_func_calls = gob_func_calls_head->total_func_calls;
    for (int i = 0; i < total_func_calls; i++)
    {
        struct goblin_function_call* gob_func_call = &gob_func_calls_head->func_calls[i];
        if (!gob_funcs_declaration_head->hasFunction(gob_func_call->func_name))
        {
            throw LinkerException("Undefined reference the function \"" + gob_func_call->func_name + "\" could not be found during the linking process");
        }

        struct goblin_function_declaration* gob_func_decl = gob_funcs_declaration_head->getFunction(gob_func_call->func_name);

        int total_func_arguments = gob_func_decl->t_arguments;
        int total_call_arguments = gob_func_call->t_arguments;
        if (total_call_arguments != total_func_arguments)
        {
            throw LinkerException("Function: \"" + gob_func_decl->func_name + "\" expects " + std::to_string(total_func_arguments) + " arguments but only " + std::to_string(total_call_arguments) + " was given");
        }
        for (int a = 0; a < gob_func_call->t_arguments; a++)
        {
            struct goblin_argument* call_argument = gob_func_call->getArgument(a);
            struct goblin_argument* func_argument = gob_func_decl->getArgument(a);
            if (call_argument->type != func_argument->type)
            {
                if (!this->getCompiler()->canCast(call_argument->type, func_argument->type))
                {
                    throw LinkerException("Function: \"" + gob_func_decl->func_name + "\" expects argument " +
                                          std::to_string(a) + " to be type " + func_argument->type +
                                          " but " + call_argument->type + " was given");
                }
            }
        }

        // Ok the function call and function are compatible!


        // Fill in the gaps where the call instruction needs to go
        code->setPosition(gob_func_call->mem_pos);
        code->setEraseMode(true);
        code->write8(CALL);
        code->write32(gob_func_decl->mem_pos);
        code->setEraseMode(false);
        code->setPosition(0);
    }

    // We are now ready to fill the executable stream
    // For now we will use raw code with no header
    while (!code->isEmpty())
    {
        executable_stream->write8(code->read8());
    }

}
