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
 * File:   FuncDefBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 18 December 2016, 23:33
 */

#ifndef FUNCDEFBRANCH_H
#define FUNCDEFBRANCH_H

#include "CustomBranch.h"

class FuncArgumentsBranch;
class EXPORT FuncDefBranch : public CustomBranch
{
public:
    FuncDefBranch(Compiler* compiler);
    FuncDefBranch(Compiler* compiler, std::string type, std::string value);
    virtual ~FuncDefBranch();

    void setReturnTypeBranch(std::shared_ptr<Branch> returnTypeBranch);
    void setNameBranch(std::shared_ptr<Branch> nameBranch);
    void setArgumentsBranch(std::shared_ptr<FuncArgumentsBranch> argumentsBranch);

    std::shared_ptr<Branch> getReturnTypeBranch();
    std::shared_ptr<Branch> getNameBranch();
    std::shared_ptr<FuncArgumentsBranch> getArgumentsBranch();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();


private:

};

#endif /* FUNCDEFBRANCH_H */

