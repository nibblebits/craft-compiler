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
 * File:   STRUCTBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 13 October 2016, 05:47
 */

#ifndef STRUCTBRANCH_H
#define STRUCTBRANCH_H

#include "CustomBranch.h"
class BODYBranch;
class EXPORT STRUCTBranch : public CustomBranch
{
public:
    STRUCTBranch(Compiler* compiler);
    virtual ~STRUCTBranch();
    
    void setStructNameBranch(std::shared_ptr<Branch> branch);
    void setStructBodyBranch(std::shared_ptr<BODYBranch> branch);
    
    std::shared_ptr<Branch> getStructNameBranch();
    std::shared_ptr<BODYBranch> getStructBodyBranch();
private:

};

#endif /* STRUCTBRANCH_H */

