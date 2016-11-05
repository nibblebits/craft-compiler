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
 * File:   VarIdentifierBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 05 November 2016, 03:05
 */

#ifndef VARIDENTIFIERBRANCH_H
#define VARIDENTIFIERBRANCH_H

#include "CustomBranch.h"

class EXPORT VarIdentifierBranch : public CustomBranch
{
public:
    VarIdentifierBranch(Compiler* compiler);
    virtual ~VarIdentifierBranch();

    void setVariableNameBranch(std::shared_ptr<Branch> name_branch);
    void setRootArrayIndexBranch(std::shared_ptr<Branch> array_index_branch);
    
    std::shared_ptr<Branch> getVariableNameBranch();
    std::shared_ptr<Branch> getRootArrayIndexBranch();
    bool hasRootArrayIndexBranch();

private:

};

#endif /* VARIDENTIFIERBRANCH_H */

