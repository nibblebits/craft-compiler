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
 * File:   SemanticValidator.h
 * Author: Daniel McCarthy
 *
 * Created on 10 December 2016, 00:07
 */

#ifndef SEMANTICVALIDATOR_H
#define SEMANTICVALIDATOR_H

#include <memory>
#include <map>
#include "CompilerEntity.h"
#include "SemanticValidatorException.h"
#include "Logger.h"

class Tree;
class Branch;
class RootBranch;
class VDEFBranch;
class BODYBranch;
class FuncBranch;
class FuncDefBranch;
class VarIdentifierBranch;
class AssignBranch;
class STRUCTDEFBranch;
class STRUCTBranch;
class EBranch;

class EXPORT SemanticValidator : public CompilerEntity
{
public:
    SemanticValidator(Compiler* compiler);
    virtual ~SemanticValidator();

    void setTree(std::shared_ptr<Tree> tree);
    void validate();

    std::shared_ptr<Logger> getLogger();
private:
    void validate_top(std::shared_ptr<RootBranch> root_branch);
    void validate_part(std::shared_ptr<Branch> branch);
    void validate_function(std::shared_ptr<FuncBranch> func_branch);
    void validate_body(std::shared_ptr<BODYBranch> body_branch);
    void validate_vdef(std::shared_ptr<VDEFBranch> vdef_branch);
    void validate_var_access(std::shared_ptr<VarIdentifierBranch> var_iden_branch);
    void validate_assignment(std::shared_ptr<AssignBranch> assign_branch);
    void validate_structure_definition(std::shared_ptr<STRUCTDEFBranch> struct_def_branch);
    void validate_structure(std::shared_ptr<STRUCTBranch> structure_branch);
    void validate_expression(std::shared_ptr<EBranch> e_branch, std::string requirement_type, bool requires_pointer);
    void validate_value(std::shared_ptr<Branch> branch, std::string requirement_type, bool requires_pointer);
    void register_function(std::shared_ptr<FuncDefBranch> func_branch);
    bool hasFunction(std::string function_name);
    std::shared_ptr<FuncDefBranch> getFunction(std::string function_name);

    void critical_error(std::string message, std::shared_ptr<Branch> branch = NULL);

    std::shared_ptr<Tree> tree;
    std::shared_ptr<Logger> logger;

    // Map of all functions
    std::map<std::string, std::shared_ptr<FuncDefBranch>> functions;
};

#endif /* SEMANTICVALIDATOR_H */

