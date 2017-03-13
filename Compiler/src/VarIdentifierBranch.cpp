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
    return getPositionRelZero([&](struct position_info * pos_info) -> void
    {
        throw Exception("Something went wrong.", "int VarIdentifierBranch::getRootPositionRelZero(POSITION_OPTIONS options)");
    }, NULL, options | POSITION_OPTION_STOP_AT_ROOT_VAR);
}

int VarIdentifierBranch::getPositionRelZero(std::function<void(struct position_info* pos_info) > handle_func, std::function<void(int rel_position) > point_func, POSITION_OPTIONS options)
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

    // We want to stop at the root var so structures and array access should be ignored.
    if (!(options & POSITION_OPTION_STOP_AT_ROOT_VAR))
    {
        struct position_info pos_info;
        pos_info.abs_start_pos = pos;
        pos = getPositionRelZeroIgnoreCurrentScope(handle_func, point_func, options, &pos_info);
    }

    return pos;
}

int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(struct position_info* pos_info) > handle_func, std::function<void(int rel_position) > point_func, POSITION_OPTIONS options, struct position_info* p_info)
{
    // We can't continue as their is no structure access branch or array index branch
    if (!hasStructureAccessBranch() && !hasRootArrayIndexBranch())
    {
        p_info->abs_pos = p_info->abs_start_pos + p_info->rel_offset_from_start_pos;
        return p_info->abs_pos;
    }
    std::shared_ptr<VarIdentifierBranch> var_to_process = std::dynamic_pointer_cast<VarIdentifierBranch>(getptr());
    if (hasStructureAccessBranch())
    {
        // We are ignoring the current scope remember.
        var_to_process = getStructureAccessBranch()->getVarIdentifierBranch();
    }

    return var_to_process->getPositionRelZeroFromThis(handle_func, point_func, options, true, p_info);

}

int VarIdentifierBranch::getPositionRelZeroFromThis(std::function<void(struct position_info* pos_info) > handle_func, std::function<void(int rel_position) > point_func, POSITION_OPTIONS options, bool is_root, struct position_info* p_info)
{
    bool keep_root = false;
    // No position information provided? then make our own.
    struct position_info new_pos_info;
    if (p_info == NULL)
    {
        p_info = &new_pos_info;
    }

    int old_abs_start_pos = p_info->abs_start_pos;
    int old_rel_offset_from_start_pos = p_info->rel_offset_from_start_pos;

    // Reset the position info
    p_info->reset();

    if (hasStructureAccessBranch() || hasRootArrayIndexBranch())
    {
        p_info->is_single = false;
    }

    int self_offset = getVariableDefinitionBranch(true)->getPositionRelScope();
    if (old_abs_start_pos == -1)
    {
        p_info->start_absolution(self_offset);
    }
    else
    {
        p_info->abs_start_pos = old_abs_start_pos;
        p_info->rel_offset_from_start_pos = old_rel_offset_from_start_pos + self_offset;
    }

    p_info->is_root = is_root;
    p_info->is_last = !hasStructureAccessBranch();
    p_info->var_iden_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(this->getptr());

    // No handle function was provided so lets just make a default one that does nothing
    if (handle_func == NULL)
    {
        handle_func = [](struct position_info * pos_info) -> void
        {

        };
    }

    // No pointer function was provided so lets just make a default one that does nothing
    if (point_func == NULL)
    {
        point_func = [](int rel_position) -> void
        {

        };
    }

    std::shared_ptr<VDEFBranch> vdef_branch = getVariableDefinitionBranch(true);
    if (vdef_branch == NULL)
    {
        throw Exception("could not find appropriate \"VDEFBranch\" for variable identifier with name: \"" + getVariableNameBranch()->getValue() + "\"", "int VarIdentifierBranch::getPositionRelZero(std::function<void(struct position_info pos_info)> handle_func, POSITION_OPTIONS options)");
    }

    p_info->data_type_size = vdef_branch->getDataTypeBranch()->getDataTypeSize();
    bool must_call_handle_func = false;
    if (hasRootArrayIndexBranch())
    {
        p_info->has_array_access = true;
        p_info->array_access_static = false;
        bool no_pointer = false;
        if (options & POSITION_OPTION_TREAT_AS_IF_NOT_POINTER)
        {
            no_pointer = true;
        }

        // When accessing pointer variables who are not arrays as arrays the position cannot be static
        if (vdef_branch->isPointer() && !vdef_branch->getVariableIdentifierBranch()->hasRootArrayIndexBranch())
        {
            no_pointer = true;
            must_call_handle_func = true;
            p_info->data_type_size = vdef_branch->getDataTypeBranch()->getDataTypeSize(no_pointer);
        }
        int size = p_info->data_type_size;
        int offset = size;
        getRootArrayIndexBranch()->iterate_array_indexes([&](std::shared_ptr<ArrayIndexBranch> array_index_branch) -> bool
        {
            if (array_index_branch->isStatic())
            {
                offset *= std::stoi(array_index_branch->getValueBranch()->getValue());
                p_info->array_access_static = true;
            }
            else
            {
                p_info->array_access_static = false;
            }
            return true;
        });

        if (p_info->array_access_static)
        {
            p_info->rel_offset_from_start_pos += offset;
        }

        p_info->array_index_branch = getRootArrayIndexBranch();
    }

    if (is_root)
    {
        if (hasStructureAccessBranch() && !getStructureAccessBranch()->isAccessingAsPointer() && (!hasRootArrayIndexBranch() || getRootArrayIndexBranch()->areAllStatic()))
        {
            keep_root = true;
        }
    }

    if (p_info->has_array_access && !p_info->array_access_static)
    {
        must_call_handle_func = true;
    }

    if (hasStructureAccessBranch() && getStructureAccessBranch()->isAccessingAsPointer())
    {
        must_call_handle_func = true;
    }

    if (keep_root)
    {
        must_call_handle_func = false;
    }


    p_info->abs_pos = p_info->abs_start_pos + p_info->rel_offset_from_start_pos;

    if (must_call_handle_func)
    {
        handle_func(p_info);
        p_info->end_absolution();
    }
    // Can we go further?
    if (hasStructureAccessBranch())
    {
        std::shared_ptr<STRUCTAccessBranch> struct_access_branch = getStructureAccessBranch();
        std::shared_ptr<VarIdentifierBranch> struct_access_var_iden_branch = struct_access_branch->getVarIdentifierBranch();
        if (struct_access_branch->isAccessingAsPointer())
        {
            if (!is_root)
            {
                point_func(p_info->abs_pos);
                p_info->end_absolution();
            }
        }

        p_info->abs_pos = struct_access_var_iden_branch->getPositionRelZeroFromThis(handle_func, point_func, options, keep_root, p_info);
    }

    return p_info->abs_pos;
}

bool VarIdentifierBranch::isPositionStatic()
{
    // When accessing pointer variables who are not arrays as arrays the position cannot be static
    if (hasRootArrayIndexBranch())
    {
        std::shared_ptr<VDEFBranch> vdef_branch = getVariableDefinitionBranch(true);
        if (vdef_branch->isPointer() && !vdef_branch->getVariableIdentifierBranch()->hasRootArrayIndexBranch())
        {
            return false;
        }
    }

    bool is_static = isAllStructureAccessStatic();
    if (is_static)
    {
        is_static = isAllArrayAccessStatic();
    }

    return is_static;
}

bool VarIdentifierBranch::isAllArrayAccessStatic()
{
    bool is_static = true;
    if (hasRootArrayIndexBranch())
    {
        is_static = getRootArrayIndexBranch()->areAllStatic();
    }

    if (is_static)
    {
        if (hasStructureAccessBranch())
        {
            is_static = getStructureAccessBranch()->getVarIdentifierBranch()->isAllArrayAccessStatic();
        }
    }
    return is_static;
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
