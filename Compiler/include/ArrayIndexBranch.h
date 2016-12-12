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
 * File:   ArrayIndex.h
 * Author: Daniel McCarthy
 *
 * Created on 04 November 2016, 02:27
 */

#ifndef ARRAYINDEXBRANCH_H
#define ARRAYINDEXBRANCH_H

#include "CustomBranch.h"

class EXPORT ArrayIndexBranch : public CustomBranch
{
public:
    ArrayIndexBranch(Compiler* compiler);
    virtual ~ArrayIndexBranch();

    void iterate_array_indexes(std::function<bool(std::shared_ptr<ArrayIndexBranch> array_index_branch) > iterate_func);
    bool isStatic();
    bool areAllStatic();
    
    void setValueBranch(std::shared_ptr<Branch> value_branch);
    void setNextArrayIndexBranch(std::shared_ptr<ArrayIndexBranch> next_array_index_branch);
    std::shared_ptr<Branch> getValueBranch();
    std::shared_ptr<ArrayIndexBranch> getNextArrayIndexBranch();

    bool hasNextArrayIndexBranch();
    bool hasParentArrayIndexBranch();
    std::shared_ptr<Branch> getDeepestArrayIndexBranch();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:

};

#endif /* ARRAYINDEXBRANCH_H */

