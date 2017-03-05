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
 * File:   STRUCTAccess.h
 * Author: Daniel McCarthy
 *
 * Created on 17 October 2016, 03:18
 */

#ifndef STRUCTACCESS_H
#define STRUCTACCESS_H

#include "CustomBranch.h"

class STRUCTDEFBranch;
class VarIdentifierBranch;

class EXPORT STRUCTAccessBranch : public CustomBranch
{
public:
    STRUCTAccessBranch(Compiler* compiler);
    virtual ~STRUCTAccessBranch();

    void setVarIdentifierBranch(std::shared_ptr<VarIdentifierBranch> var_iden_branch);
    void setAccessAsPointer(bool access_as_pointer);
    void setStructDefBranch(std::shared_ptr<STRUCTDEFBranch> struct_def_branch);
    std::shared_ptr<VarIdentifierBranch> getVarIdentifierBranch();
    bool isAccessingAsPointer();
    std::shared_ptr<STRUCTDEFBranch> getStructDefBranch();
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
    std::shared_ptr<STRUCTDEFBranch> struct_def_branch;
    bool access_as_pointer;
};

#endif /* STRUCTACCESS_H */

