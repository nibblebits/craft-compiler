/*
    Craft Compiler v0.1.0 - The standard compiler for the Craft programming language.
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
#include "VirtualObjectFormat.h"
#include "CompilerEntity.h"
#include "structs.h"
#include "Scope.h"
#include "def.h"

struct formatted_segment
{
    std::string start_segment;
    std::string end_segment;
};

typedef std::map<std::string, std::string>::iterator asm_map_it;

class EXPORT CodeGenerator : public CompilerEntity
{
public:
    CodeGenerator(Compiler* compiler, std::shared_ptr<VirtualObjectFormat> object_format, std::string code_gen_desc, int pointer_size);
    virtual ~CodeGenerator();

    void assemble();
    virtual void generate(std::shared_ptr<Tree> tree);
    virtual void assemble(std::string assembly) = 0;
    int getPointerSize();
    std::shared_ptr<VirtualObjectFormat> getObjectFormat();

protected:
    void do_asm(std::string asm_ins, std::string segment = "code");
    virtual void generate_global_branch(std::shared_ptr<Branch> branch) = 0;
    virtual struct formatted_segment format_segment(std::string segment_name) = 0;
private:
    std::shared_ptr<VirtualObjectFormat> object_format;
            
    // Key = segment, value = assembly for segment.
    std::map<std::string, std::string> assembly;
    int pointer_size;
};

#endif /* CODEGENERATOR_H */

