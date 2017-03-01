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
 * File:   FuncCall.h
 * Author: Daniel McCarthy
 *
 * Created on 19 September 2016, 22:32
 */

#ifndef FUNCCALLBRANCH_H
#define FUNCCALLBRANCH_H
#include "CustomBranch.h"

class FuncDefBranch;
class EXPORT FuncCallBranch : public CustomBranch
{
public:
    FuncCallBranch(Compiler* compiler);
    FuncCallBranch(Compiler* compiler, std::string branch_name, std::string branch_value);
    virtual ~FuncCallBranch();

    void setFuncNameBranch(std::shared_ptr<Branch> branch);
    void setFuncParamsBranch(std::shared_ptr<Branch> branch);

    std::shared_ptr<Branch> getFuncNameBranch();
    std::shared_ptr<Branch> getFuncParamsBranch();
    
    std::shared_ptr<FuncDefBranch> getFunctionDefinitionBranch();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
};

#endif /* FUNCCALLBRANCH_H */

