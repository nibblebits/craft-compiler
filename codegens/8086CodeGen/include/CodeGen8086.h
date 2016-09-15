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
 * File:   CodeGen8086.h
 * Author: Daniel McCarthy
 *
 * Created on 14 September 2016, 03:40
 */

#ifndef CODEGEN8086_H
#define CODEGEN8086_H

#define EXTERNAL_USE

#include "GoblinByteCodeLinker.h"
#include "CodeGenerator.h"

class CodeGen8086 : public CodeGenerator {
public:
    CodeGen8086(Compiler* compiler);
    virtual ~CodeGen8086();

    virtual void generateFromBranch(std::shared_ptr<Branch> branch);
    virtual std::shared_ptr<Linker> getLinker();
    virtual void scope_start(std::shared_ptr<Branch> branch);
    virtual void scope_end(std::shared_ptr<Branch> branch);
    virtual void scope_assignment(std::shared_ptr<struct variable> var, std::shared_ptr<Branch> assign_root, std::shared_ptr<Branch> assign_to);
    virtual void scope_func_call(std::shared_ptr<Branch> branch, std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments);
    virtual void scope_handle_inline_asm(std::shared_ptr<Branch> branch);
private:
    std::shared_ptr<Linker> linker;
};

#endif /* CODEGEN8086_H */

