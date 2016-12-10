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
 * File:   BODYBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 19 October 2016, 16:04
 */

#ifndef BODYBRANCH_H
#define BODYBRANCH_H

#include "ScopeBranch.h"

class EXPORT BODYBranch : public ScopeBranch
{
public:
    BODYBranch(Compiler* compiler);
    virtual ~BODYBranch();

    virtual int getScopeSize(bool include_subscopes = false, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_start = NULL, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_end = NULL, bool *should_stop = NULL);
    virtual std::shared_ptr<VDEFBranch> getVariableDefinitionBranch(std::shared_ptr<VarIdentifierBranch> var_iden, bool lookup_scope = true, bool no_follow=false);
    virtual std::shared_ptr<VDEFBranch> getVariableDefinitionBranch(std::string var_name, bool lookup_scope = true);
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:

};

#endif /* BODYBRANCH_H */

