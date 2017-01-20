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
 * File:   VDEFBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 25 June 2016, 02:31
 * 
 * Description: The branch object for a "VDEF" branch.
 */

#include "VDEFBranch.h"
#include "PTRBranch.h"
#include "VarIdentifierBranch.h"
#include "ScopeBranch.h"
#include "ArrayIndexBranch.h"

VDEFBranch::VDEFBranch(Compiler* compiler, std::string branch_name, std::string branch_value) : CustomBranch(compiler, branch_name, branch_value)
{
    this->is_pointer = false;
    this->custom_data_type_size = 0;
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

void VDEFBranch::setPointer(bool is_pointer, int depth)
{
    if (is_pointer && depth <= 0)
    {
        throw Exception("void VDEFBranch::setPointer(bool is_pointer, int depth): specifying VDEFBranch as a pointer but pointer depth is zero or below.");
    }
    this->is_pointer = is_pointer;
    this->ptr_depth = depth;
}

void VDEFBranch::setVariableType(VARIABLE_TYPE var_type)
{
    this->var_type = var_type;
}

void VDEFBranch::setCustomDataTypeSize(int size)
{
    if (this->isPointer())
    {
        throw Exception("void VDEFBranch::setCustomDataTypeSize(int size): it is unsafe to assign a custom data type size for a pointer");
    }
    
    this->custom_data_type_size = size;
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

bool VDEFBranch::hasValueExpBranch()
{
    return getValueExpBranch() != NULL;
}

std::shared_ptr<Branch> VDEFBranch::getNameBranch()
{
    std::shared_ptr<Branch> identifier_branch = getVariableIdentifierBranch();
    std::shared_ptr<VarIdentifierBranch> var_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(identifier_branch);
    return var_branch->getVariableNameBranch();
}

VARIABLE_TYPE VDEFBranch::getVariableType()
{
    return this->var_type;
}

int VDEFBranch::getPositionRelScope(POSITION_OPTIONS options)
{
    std::shared_ptr<ScopeBranch> scope_branch = getLocalScope();

    std::shared_ptr<Branch> target_branch = this->getptr();
    std::function<bool(std::shared_ptr<Branch> branch) > kill_proc = [&](std::shared_ptr<Branch> branch) -> bool
    {
        if (branch == target_branch)
            return false;

        return true;
    };

    std::function<bool(std::shared_ptr<Branch> branch) > before_proc = NULL;
    std::function<bool(std::shared_ptr<Branch> branch) > after_proc = NULL;

    if (options & POSITION_OPTION_START_WITH_VARSIZE)
    {
        after_proc = kill_proc;
    }
    else
    {
        before_proc = kill_proc;
    }

    // Get the size of all variables in this variables scope up to this variable.
    int pos = scope_branch->getScopeSize(0, before_proc, after_proc);
    return pos;
}

int VDEFBranch::getPositionRelZero(POSITION_OPTIONS options)
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

    if (options & POSITION_OPTION_START_WITH_VARSIZE)
    {
        after_proc = kill_proc;
    }
    else
    {
        before_proc = kill_proc;
    }


    // Get the size of all variables in this variables scope up to this variable.
    int pos = local_scope->getScopeSize(0, before_proc, after_proc);

    // Now get the size of all variables above up to the scope after them
    std::shared_ptr<ScopeBranch> scope_branch = local_scope;
    while (scope_branch != root_scope)
    {
        target_branch = scope_branch;
        /* Is the target branch a BODY branch? 
         * if so then we need to step up once more as this is the target we need to reach when
         * we want to stop counting.
         * 
         * Also if the BODY's parent is a FOR branch it is important to set the target branch to this. This is a bit of a hack hopefully a better solution comes up.*/
        if (target_branch->getType() == "BODY" 
                && target_branch->getParent()->getType() != "FOR")
        {
            target_branch = target_branch->getParent();
        }

        if (target_branch->getType() == "STRUCT_DEF")
        {
            // In certain situations we need to ensure that the size is not included, such as where we are getting the location of a variable in a structure
            if (options & POSITION_OPTION_START_WITH_VARSIZE)
                before_proc = kill_proc;
        }

         scope_branch = target_branch->getLocalScope();

        pos += scope_branch->getScopeSize(0, before_proc, after_proc);
    }
    return pos;

}

bool VDEFBranch::isPointer()
{
    return this->is_pointer;
}

int VDEFBranch::getPointerDepth()
{
    return this->ptr_depth;
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

bool VDEFBranch::hasCustomDataTypeSize()
{
    return this->custom_data_type_size != 0;
}

int VDEFBranch::getSize()
{
    int size = getDataTypeSize();
    std::shared_ptr<VarIdentifierBranch> var_identifier_branch = getVariableIdentifierBranch();
    if (var_identifier_branch->hasRootArrayIndexBranch())
    {
        // We have an array here.
        std::shared_ptr<ArrayIndexBranch> array_index_branch = var_identifier_branch->getRootArrayIndexBranch();
        while (true)
        {
            /* Array indexes with variable definitions are currently guaranteed to be integers. 
             * this may change in the future if I allow arrays to be declared with a variable for size */
            size *= std::stoi(array_index_branch->getValueBranch()->getValue());
            if (array_index_branch->hasNextArrayIndexBranch())
            {
                array_index_branch = array_index_branch->getNextArrayIndexBranch();
                continue;
            }

            break;
        };
    }

    return size;
}

int VDEFBranch::getDataTypeSize(bool no_pointer)
{
    if (hasCustomDataTypeSize())
        return this->custom_data_type_size;

    return this->getCompiler()->getDataTypeSizeFromVarDef(std::dynamic_pointer_cast<VDEFBranch>(this->getptr()), no_pointer);
}

int VDEFBranch::getBranchType()
{
    return BRANCH_TYPE_VDEF;
}

void VDEFBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<VDEFBranch> vdef_branch_clone = std::dynamic_pointer_cast<VDEFBranch>(cloned_branch);
    vdef_branch_clone->setDataTypeBranch(getDataTypeBranch()->clone());
    vdef_branch_clone->setVariableIdentifierBranch(getVariableIdentifierBranch()->clone());
    vdef_branch_clone->setPointer(isPointer(), getPointerDepth());
    if (hasValueExpBranch())
    {
        vdef_branch_clone->setValueExpBranch(getValueExpBranch()->clone());
    }
}

std::shared_ptr<Branch> VDEFBranch::create_clone()
{
    return std::shared_ptr<Branch>(new VDEFBranch(getCompiler(), Branch::getType(), Branch::getValue()));
}