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
 * File:   VarIdentifierBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 05 November 2016, 03:05
 * 
 * Description: 
 */

#include "VarIdentifierBranch.h"
#include "ScopeBranch.h"
#include "ArrayIndexBranch.h"
#include "STRUCTAccessBranch.h"
#include "VDEFBranch.h"

VarIdentifierBranch::VarIdentifierBranch(Compiler* compiler) : CustomBranch(compiler, "VAR_IDENTIFIER", "")
{
}

VarIdentifierBranch::~VarIdentifierBranch()
{
}

void VarIdentifierBranch::setVariableNameBranch(std::shared_ptr<Branch> name_branch)
{
    CustomBranch::registerBranch("variable_name_branch", name_branch);
}

void VarIdentifierBranch::setRootArrayIndexBranch(std::shared_ptr<Branch> array_index_branch)
{
    CustomBranch::registerBranch("root_array_index_branch", array_index_branch);
}

void VarIdentifierBranch::setStructureAccessBranch(std::shared_ptr<Branch> structure_access_branch)
{
    CustomBranch::registerBranch("structure_access_branch", structure_access_branch);
}

std::shared_ptr<Branch> VarIdentifierBranch::getVariableNameBranch()
{
    return CustomBranch::getRegisteredBranchByName("variable_name_branch");
}

bool VarIdentifierBranch::hasRootArrayIndexBranch()
{
    std::shared_ptr<Branch> root_array_index_branch = getRootArrayIndexBranch();
    return root_array_index_branch != NULL;
}

std::shared_ptr<VDEFBranch> VarIdentifierBranch::getVariableDefinitionBranch(bool no_follow)
{
    return Branch::getLocalScope()->getVariableDefinitionBranch(std::dynamic_pointer_cast<VarIdentifierBranch>(this->getptr()), true, no_follow);
}

int VarIdentifierBranch::getPositionRelZero(std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int mul_by) > unpredictable_func, bool loc_start_with_varsize)
{
    std::shared_ptr<VDEFBranch> vdef_branch = getVariableDefinitionBranch(true);
    if (unpredictable_func == NULL)
    {
        throw Exception("int VarIdentifierBranch::getPositionRelZero(std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int mul_by) > unpredictable_func, bool loc_start_with_varsize):"
                        "  You must pass an unpredictable_func so that you can generate appropriate assembly instructions for when the framework cannot calculate the position"
                        "as it is impossible to know at compile time");
    }

    // Get the position as if we are not in a scope and ignore structure access if we are a pointer
    int pos = getPositionRelZeroIgnoreCurrentScope(unpredictable_func, vdef_branch->isPointer(), loc_start_with_varsize);
    // Variables who are not alone should not start with the var size when calculating the absolute address. But only if they are not pointers
    if (!vdef_branch->isPointer() && 
            !isVariableAlone())
    {
        loc_start_with_varsize = false;
    }
    // Now add on the position up to our variable, giving us an absolute address relative to zero.
    pos += vdef_branch->getPositionRelZero(loc_start_with_varsize);
    return pos;
}

int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size) > unpredictable_func, bool ignore_structure_access, bool loc_start_with_varsize)
{
    if (unpredictable_func == NULL)
    {
        throw Exception("int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size) > unpredictable_func, bool ignore_structure_access, bool loc_start_with_varsize):"
                        "  You must pass an unpredictable_func so that you can generate appropriate assembly instructions for when the framework cannot calculate the position"
                        "as it is impossible to know at compile time");
    }

    std::shared_ptr<VDEFBranch> vdef_branch = getVariableDefinitionBranch(true);
    int pos = 0;
    if (hasRootArrayIndexBranch())
    {
        bool had_static_array = false;
        int size = vdef_branch->getDataTypeSize();
        int offset = size;
        getRootArrayIndexBranch()->iterate_array_indexes([&](std::shared_ptr<ArrayIndexBranch> array_index_branch) -> bool
        {
            if (array_index_branch->isStatic())
            {
                offset *= std::stoi(array_index_branch->getValueBranch()->getValue());
                had_static_array = true;
            }
            else
            {
                // This array index is not static, we cannot know it at runtime so lets get the programmer to fill in the gaps
                unpredictable_func(array_index_branch, size);
            }
            return true;
        });

        if (had_static_array)
        {
            pos = offset;
        }
    }

    // Ok lets get the next variable identifier(if any) and only if we are not ignoring structures
    if (!ignore_structure_access && hasStructureAccessBranch())
    {
        std::shared_ptr<STRUCTAccessBranch> struct_access_branch = getStructureAccessBranch();
        pos += struct_access_branch->getVarIdentifierBranch()->getPositionRelZeroIgnoreCurrentScope(unpredictable_func, false, loc_start_with_varsize);
        pos += struct_access_branch->getVarIdentifierBranch()->getVariableDefinitionBranch(true)->getPositionRelScope(loc_start_with_varsize);
    }
    return pos;
}

bool VarIdentifierBranch::hasStructureAccessBranch()
{
    std::shared_ptr<Branch> structure_access_branch = getStructureAccessBranch();
    return structure_access_branch != NULL;
}

bool VarIdentifierBranch::isVariableAlone()
{
    // A variable is alone when it does not have an array index or structure access branch.
    return !this->hasRootArrayIndexBranch() && !this->hasStructureAccessBranch();
}

std::shared_ptr<ArrayIndexBranch> VarIdentifierBranch::getRootArrayIndexBranch()
{
    return std::dynamic_pointer_cast<ArrayIndexBranch>(CustomBranch::getRegisteredBranchByName("root_array_index_branch"));
}

std::shared_ptr<STRUCTAccessBranch> VarIdentifierBranch::getStructureAccessBranch()
{
    return std::dynamic_pointer_cast<STRUCTAccessBranch>(CustomBranch::getRegisteredBranchByName("structure_access_branch"));
}

void VarIdentifierBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<VarIdentifierBranch> var_iden_branch_clone = std::dynamic_pointer_cast<VarIdentifierBranch>(cloned_branch);
    var_iden_branch_clone->setVariableNameBranch(getVariableNameBranch()->clone());
    if (hasStructureAccessBranch())
    {
        var_iden_branch_clone->setStructureAccessBranch(getStructureAccessBranch()->clone());
    }
}

std::shared_ptr<Branch> VarIdentifierBranch::create_clone()
{
    return std::shared_ptr<Branch>(new VarIdentifierBranch(getCompiler()));
}
