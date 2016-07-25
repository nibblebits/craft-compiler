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
 * File:   GoblinByteCodeGenerator.h
 * Author: Daniel McCarthy
 *
 * Created on 20 June 2016, 01:07
 */

#ifndef GOBLINBYTECODEGENERATOR_H
#define GOBLINBYTECODEGENERATOR_H

#include "GoblinByteCodeLinker.h"
#include "CodeGenerator.h"

// Instruction set

enum
{
};

class GoblinByteCodeGenerator : public CodeGenerator
{
public:
    GoblinByteCodeGenerator(Compiler* compiler);
    virtual ~GoblinByteCodeGenerator();

    virtual void scope_start(std::shared_ptr<Branch> branch);
    virtual void scope_assignment(std::shared_ptr<struct scope_variable> var, std::shared_ptr<Branch> assign_root, std::shared_ptr<Branch> assign_to);
    virtual void scope_func_call(std::shared_ptr<Branch> branch, std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments);
    virtual void scope_end(std::shared_ptr<Branch> branch);
    virtual void scope_exp_start();
    virtual void scope_handle_exp(std::shared_ptr<Branch> branch);
    virtual void scope_exp_end();
    virtual void scope_handle_number(std::shared_ptr<Branch> branch);
    virtual void scope_handle_identifier(std::shared_ptr<Branch> branch);
    virtual void scope_handle_inline_asm(std::shared_ptr<Branch> branch);
    virtual void scope_struct_assign_start(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> struct_ins_var, std::shared_ptr<struct scope_variable> struct_attr_var);
    virtual void scope_struct_assign_end(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> struct_ins_var, std::shared_ptr<struct scope_variable> struct_attr_var);
    virtual std::shared_ptr<Linker> getLinker();
private:

    std::shared_ptr<Linker> linker;
};

#endif /* GOBLINBYTECODEGENERATOR_H */

