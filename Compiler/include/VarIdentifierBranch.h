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

struct position
{
    position()
    {
        reset();
    }
    
    void reset()
    {
        start = 0;
        end = 0;
        abs = 0;
        do_new_start = true;
    }
    
    void calc_abs()
    {
        abs = start + end;
    }
    
    int start;
    int end;
    int abs;
    bool do_new_start;
};
struct position_info
{

    position_info()
    {
        reset();
    }

    bool has_absolution()
    {
        return this->abs_pos != -1;
    }

    void start_absolution(int abs_pos)
    {
        this->abs_start_pos = abs_pos;
        this->abs_pos = abs_pos;
        this->rel_offset_from_start_pos = 0;
    }

    void end_absolution()
    {
        this->abs_start_pos = -1;
        this->rel_offset_from_start_pos = 0;
        this->abs_pos = 0;
    }

    void reset()
    {
        is_root = false;
        is_last = false;
        is_single = true;
        has_array_access = false;
        array_access_static = false;
        point_before_array_access = false;
        abs_pos = 0;
        abs_start_pos = -1;
        rel_offset_from_start_pos = 0;
        rel_offset_from_start_pos_excluding_array_access = 0;
        data_type_size = 0;
        var_iden_branch = NULL;
    }

    /* The root is the first absolute position, its called the root as it is processed first and is different from all other processes 
     * root example: take the expression: "a.b.e->e" in that expression "a.b.e" is the root as it has to be accessed completely differently as its the first group
     * that can be loaded from an absolute position.
     */
    bool is_root;
    bool is_last;
    bool is_single;
    bool has_array_access;
    bool array_access_static;
    bool point_before_array_access;

    int abs_pos;
    int abs_start_pos;
    int rel_offset_from_start_pos;
    int rel_offset_from_start_pos_excluding_array_access;
    int data_type_size;

    std::shared_ptr<VarIdentifierBranch> var_iden_branch;
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
    bool hasVariableDefinitionBranch(bool no_follow = false);
    std::shared_ptr<VarIdentifierBranch> getFinalVarIdentifierBranch();
    int getRootPositionRelZero(POSITION_OPTIONS options = 0);
    int getPositionRelZero(std::function<void(struct position_info* pos_info) > handle_func, std::function<void(int rel_position) > point_func, POSITION_OPTIONS options = 0);
    int getPositionRelZeroIgnoreCurrentScope(std::function<void(struct position_info* pos_info) > handle_func, std::function<void(int rel_position) > point_func, POSITION_OPTIONS options = 0, struct position_info* p_info = NULL);
    int getPositionRelZeroFromThis(std::function<void(struct position_info* pos_info) > handle_func, std::function<void(int rel_position) > point_func, POSITION_OPTIONS options = 0, bool is_root = true, struct position_info* p_info = NULL);
    void getPositionAsFarAsPossible(struct position* position, std::shared_ptr<VarIdentifierBranch>* failed_var_iden_branch, POSITION_OPTIONS options = 0, bool zero_position=true);
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

