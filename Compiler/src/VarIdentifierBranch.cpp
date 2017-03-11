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
 * File:   VarIdentifierBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 05 November 2016, 03:05
 * 
 * Description: 
 */

#include "VarIdentifierBranch.h"
#include "branches.h"

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

std::shared_ptr<VarIdentifierBranch> VarIdentifierBranch::getFinalVarIdentifierBranch()
{
    // Recursive until we find the final variable identifier branch
    if (hasStructureAccessBranch())
    {
        return getStructureAccessBranch()->getVarIdentifierBranch()->getFinalVarIdentifierBranch();
    }

    return std::dynamic_pointer_cast<VarIdentifierBranch>(this->getptr());
}

int VarIdentifierBranch::getRootPositionRelZero(POSITION_OPTIONS options)
{
    return getPositionRelZero([&](int root_var_pos, bool* should_clear) -> void
    {
    }, [&](struct position_info * pos_info) -> void
    {
        throw Exception("Something went wrong.", "int VarIdentifierBranch::getRootPositionRelZero(POSITION_OPTIONS options)");
    }, options | POSITION_OPTION_STOP_AT_ROOT_VAR);
}

int VarIdentifierBranch::getPositionRelZero(std::function<void(int root_var_pos, bool* should_clear) > root_var_start_func, std::function<void(struct position_info* pos_info) > handle_func, POSITION_OPTIONS options)
{
    std::shared_ptr<VDEFBranch> vdef_branch = getVariableDefinitionBranch(true);

    if (!(options & POSITION_OPTION_POSITION_STATIC_IGNORE_HANDLE_FUNCTION))
    {
        if (handle_func == NULL)
        {
            throw Exception("expecting handle_func.", "int VarIdentifierBranch::getPositionRelZero(std::function<void(struct position_info pos_info)> handle_func, POSITION_OPTIONS options)");
        }
    }


    // Generate the position up to our variable
    int pos = vdef_branch->getPositionRelZero(options);
    if (root_var_start_func != NULL)
    {
        bool should_clear = false;
        // Invoke the root start function
        root_var_start_func(pos, &should_clear);

        if (should_clear)
        {
            pos = 0;
        }
    }

    // We want to stop at the root var so structures and array access should be ignored.
    if (!(options & POSITION_OPTION_STOP_AT_ROOT_VAR))
    {
        pos = getPositionRelZeroIgnoreCurrentScope(handle_func, options, &pos);
    }

    return pos;
}

int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(struct position_info* pos_info) > handle_func, POSITION_OPTIONS options, int* pos)
{
    struct position_info p_info;
    p_info.is_last_group = !hasStructureAccessBranch();
    if (!p_info.is_last_group)
    {
        p_info.is_last_group = !getStructureAccessBranch()->getVarIdentifierBranch()->hasStructureAccessBranch();
    }
    p_info.var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(this->getptr());
    if (handle_func == NULL)
    {
        if ((options & POSITION_OPTION_POSITION_STATIC_IGNORE_HANDLE_FUNCTION))
        {
            handle_func = [&](struct position_info * pos_info) -> void
            {
                if ((pos_info->has_array_access && !pos_info->array_access_static) || (pos_info->has_struct_access && !pos_info->struct_access_static))
                {
                    throw Exception("The option POSITION_OPTION_POSITION_STATIC_IGNORE_HANDLE_FUNCTION was passed but the position is not static", "int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(struct position_info pos_info) > handle_func, POSITION_OPTIONS options, int* pos)");
                }
            };
        }
        else
        {
            throw Exception("expecting handle_func.", "int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(int root_var_pos)> root_var_start_func, std::function<void(struct position_info pos_info)> handle_func, POSITION_OPTIONS options, int* pos)");
        }
    }


    int custom_pos = 0;
    if (pos == NULL)
    {
        // No position pointer was given so lets just create our own.
        pos = &custom_pos;
    }


    std::shared_ptr<VDEFBranch> vdef_branch = getVariableDefinitionBranch(true);
    if (vdef_branch == NULL)
    {
        throw Exception("could not find appropriate \"VDEFBranch\" for variable identifier with name: \"" + getVariableNameBranch()->getValue() + "\"", "int VarIdentifierBranch::getPositionRelZero(std::function<void(struct position_info pos_info)> handle_func, POSITION_OPTIONS options)");
    }

    if (hasRootArrayIndexBranch())
    {
        p_info.has_array_access = true;
        p_info.array_access_static = false;
        bool no_pointer = false;
        if (options & POSITION_OPTION_TREAT_AS_IF_NOT_POINTER)
        {
            no_pointer = true;
        }
        int size = vdef_branch->getDataTypeBranch()->getDataTypeSize(no_pointer);
        int offset = size;
        getRootArrayIndexBranch()->iterate_array_indexes([&](std::shared_ptr<ArrayIndexBranch> array_index_branch) -> bool
        {
            if (array_index_branch->isStatic())
            {
                offset *= std::stoi(array_index_branch->getValueBranch()->getValue());
                p_info.array_access_static = true;
            }
            else
            {
                p_info.array_access_static = false;
            }
            return true;
        });

        if (p_info.array_access_static)
        {
            *pos += offset;
            p_info.array_access_offset = offset;
        }

        p_info.array_index_branch = getRootArrayIndexBranch();
    }

    // Ok lets get the next variable identifier(if any) and only if we are not ignoring structures
    if (!(options & POSITION_OPTION_IGNORE_STRUCTURE_ACCESS) && hasStructureAccessBranch())
    {
        std::shared_ptr<STRUCTAccessBranch> struct_access_branch = getStructureAccessBranch();
        std::shared_ptr<VarIdentifierBranch> struct_access_var_iden_branch = struct_access_branch->getVarIdentifierBranch();
        // If it is the last group then the structure access is static regardless if accessing as a pointer
        if (!p_info.is_last_group && struct_access_branch->isAccessingAsPointer())
        {
            p_info.struct_access_static = false;
        }
        else
        {
            p_info.struct_access_static = true;
            p_info.struct_access_offset = struct_access_var_iden_branch->getVariableDefinitionBranch(true)->getPositionRelScope();
            *pos += p_info.struct_access_offset;
        }
        p_info.has_struct_access = true;
        p_info.struct_access_branch = struct_access_branch;
    }

    if (!p_info.has_struct_access)
    {
        // This is the last element.
        p_info.is_last = true;
    }

    p_info.abs_pos = *pos;

    // Ok lets invoke the handle function
    handle_func(&p_info);

    // Caller wants us to clear the absolute position
    if (p_info.clear_abs_pos)
    {
        *pos = 0;
    }
    else
    {
        if (p_info.clear_array_abs_pos && p_info.has_array_access && p_info.array_access_static)
        {
            *pos -= p_info.array_access_offset;
        }

        if (p_info.clear_struct_abs_pos && p_info.has_struct_access && p_info.struct_access_static)
        {
            *pos -= p_info.struct_access_static;
        }
    }

    // Can we go further?
    if (p_info.has_struct_access)
    {
        std::shared_ptr<STRUCTAccessBranch> struct_access_branch = getStructureAccessBranch();
        std::shared_ptr<VarIdentifierBranch> struct_access_var_iden_branch = struct_access_branch->getVarIdentifierBranch();
        
        if (!p_info.struct_access_static && struct_access_var_iden_branch->hasStructureAccessBranch() && !struct_access_var_iden_branch->getStructureAccessBranch()->isAccessingAsPointer())
        {
            // We are soon accessing a structure statically to the right of us because of this we need to add the current scope size to the position
            *pos += struct_access_var_iden_branch->getVariableDefinitionBranch(true)->getPositionRelScope();
        }
        *pos = struct_access_var_iden_branch->getPositionRelZeroIgnoreCurrentScope(handle_func, options, pos);
    }

    return *pos;
}

bool VarIdentifierBranch::isPositionStatic()
{
    bool is_static = isAllStructureAccessStatic();
    if (is_static)
    {
        is_static = isAllArrayAccessStatic();
    }

    return is_static;
}

bool VarIdentifierBranch::isAllArrayAccessStatic()
{
    if (hasRootArrayIndexBranch())
    {
        return getRootArrayIndexBranch()->areAllStatic();
    }

    return true;
}

bool VarIdentifierBranch::isAllStructureAccessStatic()
{
    bool is_static = true;
    // Check structure access branches
    if (hasStructureAccessBranch())
    {
        std::shared_ptr<STRUCTAccessBranch> struct_access_branch = getStructureAccessBranch();
        if (struct_access_branch->isAccessingAsPointer())
        {
            is_static = false;
        }
        else
        {
            // Lets check further if we can
            is_static = struct_access_branch->getVarIdentifierBranch()->isAllStructureAccessStatic();
        }
    }

    return is_static;
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

    if (hasRootArrayIndexBranch())
    {
        var_iden_branch_clone->setRootArrayIndexBranch(getRootArrayIndexBranch()->clone());
    }
}

std::shared_ptr<Branch> VarIdentifierBranch::create_clone()
{
    return std::shared_ptr<Branch>(new VarIdentifierBranch(getCompiler()));
}
