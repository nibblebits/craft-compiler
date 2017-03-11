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
 * File:   VarIdentifierBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 05 November 2016, 03:05
 */

#ifndef VARIDENTIFIERBRANCH_H
#define VARIDENTIFIERBRANCH_H

#include "CustomBranch.h"
class ArrayIndexBranch;
class STRUCTAccessBranch;

struct position_info
{
    position_info()
    {
        has_array_access = false;
        has_struct_access = false;
        array_access_static = false;
        struct_access_static = false;
        is_last = false;
        is_last_group = false;
        clear_abs_pos = false;
        clear_array_abs_pos = false;
        clear_struct_abs_pos = false;
        array_access_offset = 0;
        struct_access_offset = 0;
        abs_pos = 0;
        var_iden_branch = NULL;
        struct_access_branch = NULL;
        array_index_branch = NULL;
    }
    bool has_array_access;
    bool has_struct_access;
    bool array_access_static;
    bool struct_access_static;
    bool is_last;
    bool is_last_group;
    bool clear_abs_pos;
    bool clear_array_abs_pos;
    bool clear_struct_abs_pos;
    int array_access_offset;
    int struct_access_offset;
    int abs_pos;
    std::shared_ptr<VarIdentifierBranch> var_iden_branch;
    std::shared_ptr<STRUCTAccessBranch> struct_access_branch;
    std::shared_ptr<ArrayIndexBranch> array_index_branch;
};

class EXPORT VarIdentifierBranch : public CustomBranch
{
public:
    VarIdentifierBranch(Compiler* compiler);
    virtual ~VarIdentifierBranch();

    void setVariableNameBranch(std::shared_ptr<Branch> name_branch);
    void setRootArrayIndexBranch(std::shared_ptr<Branch> array_index_branch);
    void setStructureAccessBranch(std::shared_ptr<Branch> structure_access_branch);

    std::shared_ptr<Branch> getVariableNameBranch();
    std::shared_ptr<STRUCTAccessBranch> getStructureAccessBranch();
    std::shared_ptr<ArrayIndexBranch> getRootArrayIndexBranch();
    std::shared_ptr<VDEFBranch> getVariableDefinitionBranch(bool no_follow = false);
    std::shared_ptr<VarIdentifierBranch> getFinalVarIdentifierBranch();
    int getRootPositionRelZero(POSITION_OPTIONS options = 0);
    int getPositionRelZero(std::function<void(int root_var_pos, bool* should_clear)> root_var_start_func, std::function<void(struct position_info* pos_info)> handle_func, POSITION_OPTIONS options = 0);
    int getPositionRelZeroIgnoreCurrentScope(std::function<void(struct position_info* pos_info)> handle_func, POSITION_OPTIONS options = 0, int* pos = NULL);
    bool isPositionStatic();
    bool isAllArrayAccessStatic();
    bool isAllStructureAccessStatic();
    bool hasRootArrayIndexBranch();
    bool hasStructureAccessBranch();
    bool isVariableAlone();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:

};

#endif /* VARIDENTIFIERBRANCH_H */

