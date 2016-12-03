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

int VDEFBranch::getPositionRelZero(bool loc_start_with_filesize)
{
    std::shared_ptr<ScopeBranch> root_scope = getRootScope();
    std::shared_ptr<ScopeBranch> local_scope = getLocalScope();
    std::shared_ptr<Branch> target_branch = this->getptr();
    std::function<bool(std::shared_ptr<Branch> branch) > kill_proc = [&](std::shared_ptr<Branch> branch) -> bool
    {
        if (branch == target_branch)
            return false;

        return true;
    };

    std::function<bool(std::shared_ptr<Branch> branch) > before_proc = NULL;
    std::function<bool(std::shared_ptr<Branch> branch) > after_proc = NULL;

    if (loc_start_with_filesize)
    {
        after_proc = kill_proc;
    }
    else
    {
        before_proc = kill_proc;
    }

    // Get the size of all variables in this variables scope up to this variable.
    int pos = local_scope->getScopeSize(false, before_proc, after_proc);

    // Now get the size of all variables above up to the scope after them
    std::shared_ptr<ScopeBranch> scope_branch = local_scope;
    while (scope_branch != root_scope)
    {
        target_branch = scope_branch;
        /* Is the target branch a BODY branch? 
         * if so then we need to step up once more as this is the target we need to reach when
         * we want to stop counting */
        if (target_branch->getType() == "BODY")
        {
            target_branch = target_branch->getParent();
        }
        scope_branch = target_branch->getLocalScope();
        pos += scope_branch->getScopeSize(false, before_proc, after_proc);
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
