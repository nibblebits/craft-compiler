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
 * File:   StandardScopeBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 15 December 2016, 18:19
 */

#ifndef STANDARDSCOPEBRANCH_H
#define STANDARDSCOPEBRANCH_H

#include "ScopeBranch.h"

class StandardScopeBranch : public ScopeBranch
{
public:
    StandardScopeBranch(Compiler* compiler, std::string name, std::string value);
    virtual ~StandardScopeBranch();

    int getScopeSize(GET_SCOPE_SIZE_OPTIONS options=0, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_start = NULL, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_end = NULL, bool *should_stop = NULL);
    std::shared_ptr<VDEFBranch> getVariableDefinitionBranch(std::shared_ptr<VarIdentifierBranch> var_iden, bool lookup_scope = true, bool no_follow = false);
    std::shared_ptr<VDEFBranch> getVariableDefinitionBranch(std::string var_name, bool lookup_scope = true);

private:

};

#endif /* STANDARDSCOPEBRANCH_H */

