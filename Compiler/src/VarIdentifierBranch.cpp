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

int VarIdentifierBranch::getRootPositionRelZero(POSITION_OPTIONS options)
{
    return getPositionRelZero([&](int pos, std::shared_ptr<VarIdentifierBranch> var_iden_branch, bool is_root_var) -> void
    {
        throw Exception("Something went wrong calculating an absolute root position", "int VarIdentifierBranch::getRootPositionRelZero()");
    },
                              [&](std::shared_ptr<ArrayIndexBranch> array_index_branch, int mul_by)
                              {
                              }, [&](std::shared_ptr<VarIdentifierBranch> left_var_iden, std::shared_ptr<VarIdentifierBranch> right_var_iden) -> void
                              {
                              }, options | POSITION_OPTION_STOP_AT_ROOT_VAR);
}

/**
 * Gets the position of this variable related to zero and where it cannot get the position at compile time as it is not possible
 * invokes the "abs_gen_func", and will also invoke the "array_unpredictable_func" or the "struct_access_unpredictable_func" function depending
 * on the type of position that could not be calculated.
 * 
 * The "abs_gen_func" will be called when ever it is not possible to calculate the position at compile time.
 * Once invoked you will have access to two arguments one argument being the current absolute position and the other being the current VarIdentifierBranch.
 * After calling the "abs_gen_func" the absolute position will be reset to zero.
 * 
 * Only the last absolute position is returned. The "abs_gen_func" will not be called for the last absolute position and will only be called
 * when it is no longer possible to maintain an absolute position. 
 * 
 * <b>This method is a method to assist the code generator in resolving positions for variables</b>
 * 
 * @param abs_gen_func - The function that should be called to handle the current absolute position
 * @param array_unpredictable_func - The function that should handle array access that cannot be handled at compile time
 * @param struct_access_unpredictable_func - The function that should handle structure access that cannot be handled at compile time
 * @param options - The options for this operation
 * @return The last absolute position for this operation.
 */
int VarIdentifierBranch::getPositionRelZero(std::function<void(int pos, std::shared_ptr<VarIdentifierBranch> var_iden_branch, bool is_root_var) > abs_gen_func, std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int mul_by) > array_unpredictable_func, std::function<void( std::shared_ptr<VarIdentifierBranch> left_var_iden, std::shared_ptr<VarIdentifierBranch> right_var_iden) > struct_access_unpredictable_func, POSITION_OPTIONS options)
{
    std::shared_ptr<VDEFBranch> vdef_branch = getVariableDefinitionBranch(true);
    if (array_unpredictable_func == NULL)
    {
        throw Exception("int VarIdentifierBranch::getPositionRelZero(std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int mul_by) > array_unpredictable_func,std::function<void( std::shared_ptr<VarIdentifierBranch> left_var_iden, std::shared_ptr<VarIdentifierBranch> right_var_iden)> struct_access_unpredictable_func, POSITION_OPTIONS options):"
                        "  You must pass an array_unpredictable_func so that you can generate appropriate assembly instructions for when the framework cannot calculate the position"
                        "as it is impossible to know at compile time");
    }

    if (struct_access_unpredictable_func == NULL)
    {
        throw Exception("int VarIdentifierBranch::getPositionRelZero(std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int mul_by) > array_unpredictable_func,std::function<void( std::shared_ptr<VarIdentifierBranch> left_var_iden, std::shared_ptr<VarIdentifierBranch> right_var_iden)> struct_access_unpredictable_func, POSITION_OPTIONS options):"
                        "  You must pass a struct_access_unpredictable_func so that you can generate appropriate assembly instructions for when the framework cannot calculate the position"
                        "as it is impossible to know at compile time");
    }

    // Generate the position up to our variable
    int pos = vdef_branch->getPositionRelZero(options);
    
    // We want to stop at the root var so structures and array access should be ignored.
    if (!(options & POSITION_OPTION_STOP_AT_ROOT_VAR))
    {
        pos = getPositionRelZeroIgnoreCurrentScope(abs_gen_func, array_unpredictable_func, struct_access_unpredictable_func, options, &pos, true);
    }

    return pos;
}

int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(int pos, std::shared_ptr<VarIdentifierBranch> var_iden_branch, bool is_root_var) > abs_gen_func, std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size) > array_unpredictable_func, std::function<void( std::shared_ptr<VarIdentifierBranch> left_var_iden, std::shared_ptr<VarIdentifierBranch> right_var_iden) > struct_access_unpredictable_func, POSITION_OPTIONS options, int* pos, bool is_root_var)
{
    if (array_unpredictable_func == NULL)
    {
        throw Exception("int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size) > array_unpredictable_func, std::function<void( std::shared_ptr<VarIdentifierBranch> left_var_iden, std::shared_ptr<VarIdentifierBranch> right_var_iden) > struct_access_unpredictable_func, POSITION_OPTIONS options):"
                        "  You must pass an array_unpredictable_func so that you can generate appropriate assembly instructions for when the framework cannot calculate the position"
                        "as it is impossible to know at compile time");
    }

    if (struct_access_unpredictable_func == NULL)
    {
        throw Exception("int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size) > array_unpredictable_func, std::function<void( std::shared_ptr<VarIdentifierBranch> left_var_iden, std::shared_ptr<VarIdentifierBranch> right_var_iden) > struct_access_unpredictable_func, POSITION_OPTIONS options):"
                        "  You must pass a struct_access_unpredictable_func so that you can generate appropriate assembly instructions for when the framework cannot calculate the position"
                        "as it is impossible to know at compile time");
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
        throw Exception("int VarIdentifierBranch::getPositionRelZeroIgnoreCurrentScope(std::function<void(std::shared_ptr<ArrayIndexBranch> array_index_branch, int elem_size) > array_unpredictable_func, POSITION_OPTIONS options): "
                        "could not find appropriate \"VDEFBranch\" for variable identifier with name: \"" + getVariableNameBranch()->getValue() + "\"");
    }
    if (hasRootArrayIndexBranch())
    {
        bool has_static_array = false;
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
                has_static_array = true;
            }
            else
            {
                has_static_array = false;
                if (*pos + offset != 0)
                {
                    // We are at something impossible to know at compile time so lets invoke the absolute generation function so that it can handle anything previously done
                    abs_gen_func(*pos + offset, std::dynamic_pointer_cast<VarIdentifierBranch>(this->getptr()), is_root_var);
                    is_root_var = false;
                    *pos = 0;
                    offset = 0;
                }
                // This array index is not static, we cannot know it at compile time so lets get the programmer to fill in the gaps
                array_unpredictable_func(array_index_branch, size);
            }
            return true;
        });

        if (has_static_array)
        {
            *pos += offset;
        }
    }

    // Ok lets get the next variable identifier(if any) and only if we are not ignoring structures
    if (!(options & POSITION_OPTION_IGNORE_STRUCTURE_ACCESS) && hasStructureAccessBranch())
    {
        // When accessing a structure we should not start with the variable size
        //options &= ~POSITION_OPTION_START_WITH_VARSIZE;
        std::shared_ptr<STRUCTAccessBranch> struct_access_branch = getStructureAccessBranch();
        if (struct_access_branch->isAccessingAsPointer())
        {
            if (*pos != 0)
            {
                // We should generate any absolute position we are aware of
                abs_gen_func(*pos, std::dynamic_pointer_cast<VarIdentifierBranch>(this->getptr()), is_root_var);
                *pos = 0;
            }

            // It may be possible for this to have an absolute position if it is the last struct access, lets find out.
            if (!struct_access_branch->getVarIdentifierBranch()->hasStructureAccessBranch())
            {
                // Ok we are all good we can make this an absolute position
                *pos = struct_access_branch->getVarIdentifierBranch()->getVariableDefinitionBranch(true)->getPositionRelScope();
            }
            else
            {
                // We are accessing this structure as a pointer, its impossible to know the address at compile time so we need to invoke the struct_access_unpredictable_func
                struct_access_unpredictable_func(std::dynamic_pointer_cast<VarIdentifierBranch>(this->getptr()), struct_access_branch->getVarIdentifierBranch());
                *pos = struct_access_branch->getVarIdentifierBranch()->getPositionRelZeroIgnoreCurrentScope(abs_gen_func, array_unpredictable_func, struct_access_unpredictable_func, options, pos, false);
            }
        }
        else
        {
            *pos = struct_access_branch->getVarIdentifierBranch()->getVariableDefinitionBranch(true)->getPositionRelScope(options);
            *pos = struct_access_branch->getVarIdentifierBranch()->getPositionRelZeroIgnoreCurrentScope(abs_gen_func, array_unpredictable_func, struct_access_unpredictable_func, options, pos, is_root_var);
        }
    }

    return *pos;
}

bool VarIdentifierBranch::isPositionStatic()
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
            is_static = struct_access_branch->getVarIdentifierBranch()->isPositionStatic();
        }
    }

    if (is_static)
    {
        // We are still static so lets check the array indexes
        if (hasRootArrayIndexBranch())
        {
            is_static = getRootArrayIndexBranch()->areAllStatic();
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
