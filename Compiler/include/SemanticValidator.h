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

struct semantic_value_information
{

    semantic_value_information()
    {
        requirement_type = "";
        requires_pointer = false;
        pointer_depth = 0;
    }
    std::string requirement_type;
    bool requires_pointer;
    int pointer_depth;
};

struct semantic_information
{
    struct semantic_value_information sv_info;
};

class Tree;
class Branch;
class RootBranch;
class VDEFBranch;
class ReturnBranch;
class BODYBranch;
class FuncBranch;
class FuncDefBranch;
class FuncCallBranch;
class VarIdentifierBranch;
class PTRBranch;
class AssignBranch;
class FORBranch;
class STRUCTDEFBranch;
class STRUCTBranch;
class EBranch;
class ASMBranch;

class EXPORT SemanticValidator : public CompilerEntity
{
public:
    SemanticValidator(Compiler* compiler);
    virtual ~SemanticValidator();

    void allow_data_type(std::string data_type);
    bool is_data_type_allowed(std::string data_type);

    void setTree(std::shared_ptr<Tree> tree);
    void validate();

    std::shared_ptr<Logger> getLogger();
private:
    void validate_top(std::shared_ptr<RootBranch> root_branch);
    void validate_part(std::shared_ptr<Branch> branch);
    void validate_function_definition(std::shared_ptr<FuncDefBranch> func_def_branch);
    void validate_function(std::shared_ptr<FuncBranch> func_branch);
    void validate_body(std::shared_ptr<BODYBranch> body_branch);
    void validate_vdef(std::shared_ptr<VDEFBranch> vdef_branch);
    void validate_return(std::shared_ptr<ReturnBranch> return_branch);
    void validate_var_access(std::shared_ptr<VarIdentifierBranch> var_iden_branch);
    bool validate_pointer_access(std::shared_ptr<PTRBranch> ptr_branch);
    void validate_assignment(std::shared_ptr<AssignBranch> assign_branch);
    void validate_for_stmt(std::shared_ptr<FORBranch> for_branch);
    void validate_function_call(std::shared_ptr<FuncCallBranch> func_call_branch);
    void validate_structure_definition(std::shared_ptr<STRUCTDEFBranch> struct_def_branch);
    void validate_structure(std::shared_ptr<STRUCTBranch> structure_branch);
    void validate_expression(std::shared_ptr<EBranch> e_branch, struct semantic_information* s_info);
    void validate_value(std::shared_ptr<Branch> branch, struct semantic_information* s_info);
    void validate_inline_asm(std::shared_ptr<ASMBranch> asm_branch);
    void register_function(std::shared_ptr<FuncDefBranch> func_branch);
    bool hasFunction(std::string function_name);
    bool hasFunctionDeclaration(std::string function_name);
    std::shared_ptr<FuncDefBranch> getFunction(std::string function_name);

    bool ensure_function_exists(std::string func_name, std::shared_ptr<Branch> stmt_branch);
    bool ensure_variable_valid(std::shared_ptr<VarIdentifierBranch> var_iden_branch);
    bool ensure_variable_exists(std::shared_ptr<VarIdentifierBranch> var_iden_branch);
    bool ensure_variable_not_already_registered_in_local_scope(std::shared_ptr<VDEFBranch> vdef_branch);
    bool ensure_variable_type_legal(std::string var_type, std::shared_ptr<Branch> branch = NULL);

    void function_not_declared(std::string func_name, std::shared_ptr<Branch> stmt_branch);
    void variable_not_declared(std::shared_ptr<VarIdentifierBranch> var_iden_branch);
    void critical_error(std::string message, std::shared_ptr<Branch> branch = NULL);

    std::shared_ptr<Tree> tree;
    std::shared_ptr<Logger> logger;

    // Map of all functions
    std::map<std::string, std::shared_ptr<FuncDefBranch>> functions;
    
    // The current function that is being validated
    std::shared_ptr<FuncBranch> current_function;

    // Allowed data types
    std::vector<std::string> allowed_data_types;
};

#endif /* SEMANTICVALIDATOR_H */

