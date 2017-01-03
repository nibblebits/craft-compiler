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
 * File:   FuncBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 25 June 2016, 01:30
 */

#ifndef FUNCBRANCH_H
#define FUNCBRANCH_H

#include "FuncDefBranch.h"

class BODYBranch;
class EXPORT FuncBranch : public FuncDefBranch
{
public:
    FuncBranch(Compiler* compiler);
    virtual ~FuncBranch();
   
    void setBodyBranch(std::shared_ptr<BODYBranch> bodyBranch);
    std::shared_ptr<BODYBranch> getBodyBranch();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:

};

#endif /* FUNCBRANCH_H */

