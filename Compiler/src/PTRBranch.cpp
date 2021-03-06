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
 * File:   PtrBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 21 September 2016, 23:59
 * 
 * Description: 
 */

#include "PTRBranch.h"
#include "VarIdentifierBranch.h"
#include "VDEFBranch.h"

PTRBranch::PTRBranch(Compiler* compiler) : CustomBranch(compiler, "PTR", "")
{
}

PTRBranch::~PTRBranch()
{
}

void PTRBranch::setExpressionBranch(std::shared_ptr<Branch> var_branch)
{
    CustomBranch::registerBranch("expression_branch", var_branch);
}

std::shared_ptr<Branch> PTRBranch::getExpressionBranch()
{
    return CustomBranch::getRegisteredBranchByName("expression_branch");
}

/**
 * Returns a variable identifier branch for the pointer variable that this pointer is referencing to or returns NULL if no such
 * pointer variable was found.
 * 
 * @return The VarIdentifierBranch object for the pointer variable that this pointer is referencing too
 */
std::shared_ptr<VarIdentifierBranch> PTRBranch::getPointerVariableIdentifierBranch()
{
    std::shared_ptr<VarIdentifierBranch> ptr_var_iden_branch = NULL;

    // Search for the pointer variable identifier
    std::function<void(std::shared_ptr<Branch> branch) > iterate_func = [&](std::shared_ptr<Branch> branch) -> void
    {
        if (branch->getType() == "E")
        {
            branch->iterate_children(iterate_func);
        }
        else if (branch->getType() == "VAR_IDENTIFIER")
        {
            std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(branch);
            std::shared_ptr<VDEFBranch> vdef_branch = var_iden_branch->getVariableDefinitionBranch(true);
            if (vdef_branch->isPointer())
                ptr_var_iden_branch = var_iden_branch;
        }
    };

    Branch::iterate_children(iterate_func);

    return ptr_var_iden_branch;
}

/**
 * Returns a variable identifier branch for the pointer variable that this pointer is referencing to or throws an exception if no such
 * pointer variable was found.
 * 
 * @throws Exception - Throws an exception if no pointer variable identifier branch was found
 * @return The VarIdentifierBranch object for the pointer variable that this pointer is referencing too
 */
std::shared_ptr<VarIdentifierBranch> PTRBranch::getFirstPointerVariableIdentifierBranch()
{
    std::shared_ptr<VarIdentifierBranch> ptr_var_iden_branch = NULL;

    // Search for the pointer variable identifier
    std::function<void(std::shared_ptr<Branch> branch) > iterate_func = [&](std::shared_ptr<Branch> branch) -> void
    {
        if (branch->getType() == "E")
        {
            branch->iterate_children(iterate_func);
        }
        else if (branch->getType() == "VAR_IDENTIFIER")
        {
            std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(branch);
            if (var_iden_branch->hasVariableDefinitionBranch(true))
            {
                std::shared_ptr<VDEFBranch> vdef_branch = var_iden_branch->getVariableDefinitionBranch(true);
                if (vdef_branch->isPointer())
                    ptr_var_iden_branch = var_iden_branch;
            }
        }
    };

    Branch::iterate_children(iterate_func);

    if (ptr_var_iden_branch == NULL)
    {
        throw Exception("This PTRBranch has no variable pointer identifier branch associated to it", "std::shared_ptr<VarIdentifierBranch> PTRBranch::getFirstVariablePointerIdentifierBranch()");
    }
    return ptr_var_iden_branch;
}

/**
 * Returns weather or not this PTRBranch has a VariableIdentifierBranch that is a pointer.
 * 
 * @throws Exception - Throws an exception if no pointer variable identifier branch was found
 * @return The VarIdentifierBranch object for the pointer variable that this pointer is referencing too
 */
bool PTRBranch::hasPointerVariableIdentifierBranch()
{
    std::shared_ptr<VarIdentifierBranch> ptr_var_iden_branch = NULL;

    // Search for the pointer variable identifier
    std::function<void(std::shared_ptr<Branch> branch) > iterate_func = [&](std::shared_ptr<Branch> branch) -> void
    {
        if (branch->getType() == "E")
        {
            branch->iterate_children(iterate_func);
        }
        else if (branch->getType() == "VAR_IDENTIFIER")
        {
            std::shared_ptr<VarIdentifierBranch> var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(branch);
            if (var_iden_branch->hasVariableDefinitionBranch(true))
            {
                std::shared_ptr<VDEFBranch> vdef_branch = var_iden_branch->getVariableDefinitionBranch(true);
                if (vdef_branch->isPointer())
                    ptr_var_iden_branch = var_iden_branch;
            }
        }
    };

    Branch::iterate_children(iterate_func);

    return ptr_var_iden_branch != NULL;
}

void PTRBranch::setPointerDepth(int depth)
{
    this->ptr_depth = depth;
}

int PTRBranch::getPointerDepth()
{
    return this->ptr_depth;
}

void PTRBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<PTRBranch> ptr_branch_clone = std::dynamic_pointer_cast<PTRBranch>(cloned_branch);
    ptr_branch_clone->setExpressionBranch(getExpressionBranch()->clone());
}

std::shared_ptr<Branch> PTRBranch::create_clone()
{
    return std::shared_ptr<Branch>(new PTRBranch(getCompiler()));
}
