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
 * File:   FORBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 20 October 2016, 04:19
 */

#ifndef FORBRANCH_H
#define FORBRANCH_H

#include "ScopeBranch.h"

class BODYBranch;
class EXPORT FORBranch : public ScopeBranch
{
public:
    FORBranch(Compiler* compiler);
    virtual ~FORBranch();

    void setInitBranch(std::shared_ptr<Branch> branch);
    void setCondBranch(std::shared_ptr<Branch> branch);
    void setLoopBranch(std::shared_ptr<Branch> branch);
    void setBodyBranch(std::shared_ptr<BODYBranch> branch);

    std::shared_ptr<Branch> getInitBranch();
    std::shared_ptr<Branch> getCondBranch();
    std::shared_ptr<Branch> getLoopBranch();
    std::shared_ptr<BODYBranch> getBodyBranch();

    virtual int getScopeSize(bool include_subscopes=false, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_start = NULL, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_end = NULL, bool *should_stop=NULL);
private:

};

#endif /* FORBRANCH_H */

