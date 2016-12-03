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
 * File:   VDEFBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 25 June 2016, 02:31
 * 
 * Description: The branch object for a "VDEF" branch.
 */

#include "VDEFBranch.h"
#include "ArrayBranch.h"
#include "PTRBranch.h"
#include "VarIdentifierBranch.h"
#include "ScopeBranch.h"

VDEFBranch::VDEFBranch(Compiler* compiler, std::string branch_name, std::string branch_value) : CustomBranch(compiler, branch_name, branch_value)
{
    this->is_pointer = false;
}

VDEFBranch::~VDEFBranch()
{
}

void VDEFBranch::setDataTypeBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("data_type_branch", branch);
}

void VDEFBranch::setVariableIdentifierBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("var_identifier_branch", branch);
}

void VDEFBranch::setValueExpBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("value_exp_branch", branch);
}

void VDEFBranch::setPointer(bool is_pointer)
{
    this->is_pointer = is_pointer;
}

std::shared_ptr<Branch> VDEFBranch::getDataTypeBranch()
{
    return this->getRegisteredBranchByName("data_type_branch");
}

std::shared_ptr<VarIdentifierBranch> VDEFBranch::getVariableIdentifierBranch()
{
    return std::dynamic_pointer_cast<VarIdentifierBranch>(this->getRegisteredBranchByName("var_identifier_branch"));
}

std::shared_ptr<Branch> VDEFBranch::getValueExpBranch()
{
    return this->getRegisteredBranchByName("value_exp_branch");
}

std::shared_ptr<Branch> VDEFBranch::getNameBranch()
{
    std::shared_ptr<Branch> identifier_branch = getVariableIdentifierBranch();
    std::shared_ptr<VarIdentifierBranch> var_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(identifier_branch);
    return var_branch->getVariableNameBranch();
}

int VDEFBranch::getPositionRelZero(std::function<void(std::shared_ptr<VDEFBranch> current_vdef, int *offset_adjustment) > position_alignment_proc)
{
    std::shared_ptr<ScopeBranch> root_scope = getRootScope();
    std::shared_ptr<ScopeBranch> local_scope = getLocalScope();
    // Get the size of all variables in this variables scope up to this variable.
    int pos = 0;
    int offset = 0;
    pos = local_scope->getScopeSize(false, [&](std::shared_ptr<Branch> branch) -> bool
    {
        // This allows for realignment of the position in real time
        // Consider providing better functionality at a later date as this is probably not a very fast or efficient way of doing things
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        if (vdef_branch != NULL &&
            position_alignment_proc != NULL)
        {
            position_alignment_proc(vdef_branch, &offset);
        }
        
        if (branch == this->getptr())
            return false;

        return true;
    });
    
    pos += offset;
    
    // Now get the size of all variables above up to the scope after them
    std::shared_ptr<ScopeBranch> scope_branch = local_scope;
    std::shared_ptr<ScopeBranch> prev_scope_branch = NULL;
    while (scope_branch != root_scope)
    {
        prev_scope_branch = scope_branch;
        scope_branch = prev_scope_branch->getLocalScope();
        offset = 0;
        pos += scope_branch->getScopeSize(false, [&](std::shared_ptr<Branch> branch) -> bool
        {
            // This allows for realignment of the position in real time
            // Consider providing better functionality at a later date as this is probably not a very fast or efficient way of doing things
            std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
            if (vdef_branch != NULL &&
                position_alignment_proc != NULL)
            {
                position_alignment_proc(vdef_branch, &offset);
            }
            
            if (branch == prev_scope_branch)
                return false;
            
            return true;
        });
        pos += offset;
    }
    return pos;
}

bool VDEFBranch::isPointer()
{
    return this->is_pointer;
}

bool VDEFBranch::isSigned()
{
    std::shared_ptr<Branch> data_type_branch = getDataTypeBranch();
    std::string data_type_value = data_type_branch->getValue();
    return (
            data_type_value == "int8" ||
            data_type_value == "int16" ||
            data_type_value == "int32" ||
            data_type_value == "int64"
            );
}

bool VDEFBranch::isPrimitive()
{
    std::shared_ptr<Branch> data_type_branch = getDataTypeBranch();
    return this->getCompiler()->isPrimitiveDataType(data_type_branch->getValue());
}

int VDEFBranch::getDataTypeSize(bool no_pointer)
{
    return this->getCompiler()->getDataTypeSizeFromVarDef(std::dynamic_pointer_cast<VDEFBranch>(this->getptr()), no_pointer);
}
