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
 * File:   AddressOfBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 20 September 2016, 21:27
 */

#ifndef ADDRESSOFBRANCH_H
#define ADDRESSOFBRANCH_H
#include "CustomBranch.h"

class EXPORT AddressOfBranch : public CustomBranch
{
public:
    AddressOfBranch(Compiler* compiler);
    virtual ~AddressOfBranch();

    void setVariableBranch(std::shared_ptr<Branch> branch);
    std::shared_ptr<Branch> getVariableBranch();
    
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();
private:

};

#endif /* ADDRESSOFBRANCH_H */

