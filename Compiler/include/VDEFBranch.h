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
 * File:   VDEFBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 25 June 2016, 02:31
 */

#ifndef VDEFBRANCH_H
#define VDEFBRANCH_H

#include "CustomBranch.h"
#include "VarIdentifierBranch.h"

class EXPORT VDEFBranch : public CustomBranch
{
public:
    VDEFBranch(Compiler* compiler, std::string branch_name = "V_DEF", std::string branch_value = "");
    virtual ~VDEFBranch();

    void setDataTypeBranch(std::shared_ptr<Branch> branch);
    void setVariableIdentifierBranch(std::shared_ptr<Branch> branch);
    void setValueExpBranch(std::shared_ptr<Branch> branch);
    void setPointer(bool is_pointer);
    void setVariableType(VARIABLE_TYPE var_type);

    std::shared_ptr<Branch> getDataTypeBranch();
    std::shared_ptr<VarIdentifierBranch> getVariableIdentifierBranch();
    std::shared_ptr<Branch> getValueExpBranch();
    std::shared_ptr<Branch> getNameBranch();
    VARIABLE_TYPE getVariableType();

    bool hasValueExpBranch();
    int getPositionRelScope(bool loc_start_with_varsize = false);
    int getPositionRelZero(bool loc_start_with_varsize = false);

    bool isPointer();
    bool isSigned();
    bool isPrimitive();

    int getSize();
    virtual int getDataTypeSize(bool no_pointer = false);
    virtual int getBranchType();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
    VARIABLE_TYPE var_type;
    bool is_pointer;
};

#endif /* VDEFBRANCH_H */

