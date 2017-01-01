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
 * File:   STRUCTDEFBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 14 October 2016, 04:20
 */

#ifndef STRUCTDEFBRANCH_H
#define STRUCTDEFBRANCH_H

#include "VDEFBranch.h"

class BODYBranch;

class EXPORT STRUCTDEFBranch : public VDEFBranch
{
public:
    STRUCTDEFBranch(Compiler* compiler);
    virtual ~STRUCTDEFBranch();

    // The unique cloned structure's body branch unique to this structure definition
    void setStructBody(std::shared_ptr<BODYBranch> struct_body_branch);
    std::shared_ptr<BODYBranch> getStructBody();
    int getBranchType();
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
    std::shared_ptr<BODYBranch> unique_struct_body_branch;
};

#endif /* STRUCTDEFBRANCH_H */

