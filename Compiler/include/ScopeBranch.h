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
 * File:   ScopeBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 01 December 2016, 17:25
 */

#ifndef SCOPEBRANCH_H
#define SCOPEBRANCH_H

#include "CustomBranch.h"

class VDEFBranch;
class VarIdentifierBranch;

class EXPORT ScopeBranch : public CustomBranch
{
public:
    ScopeBranch(Compiler* compiler, std::string name, std::string value);
    virtual ~ScopeBranch();
    virtual int getScopeSize(GET_SCOPE_SIZE_OPTIONS options = 0, std::function<bool(std::shared_ptr<Branch> child_branch) > elem_proc_start = NULL, std::function<bool(std::shared_ptr<Branch> child_branch) > elem_proc_end = NULL, bool *should_stop = NULL) = 0;
    virtual std::shared_ptr<VDEFBranch> getVariableDefinitionBranch(std::shared_ptr<VarIdentifierBranch> var_iden, bool lookup_scope = true, bool no_follow = false) = 0;
    virtual std::shared_ptr<VDEFBranch> getVariableDefinitionBranch(std::string var_name, bool lookup_scope = true) = 0;

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch) = 0;
    virtual std::shared_ptr<Branch> create_clone() = 0;
protected:
    bool invoke_scope_size_proc_if_possible(std::function<bool(std::shared_ptr<Branch> child_branch) > elem_proc, std::shared_ptr<Branch> child, bool* should_stop);
private:

};

#endif /* SCOPEBRANCH_H */

