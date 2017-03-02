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
 * File:   PtrBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 21 September 2016, 23:59
 */

#ifndef PTRBRANCH_H
#define PTRBRANCH_H

#include "CustomBranch.h"

class EXPORT PTRBranch : public CustomBranch
{
public:
    PTRBranch(Compiler* compiler);
    virtual ~PTRBranch();
    void setExpressionBranch(std::shared_ptr<Branch> var_branch);
    std::shared_ptr<Branch> getExpressionBranch();
    
    std::shared_ptr<VarIdentifierBranch> getPointerVariableIdentifierBranch();

    void setPointerDepth(int depth);
    int getPointerDepth();
    
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
    int ptr_depth;
};

#endif /* PTRBRANCH_H */

