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
 * File:   IFBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 02 October 2016, 03:20
 */

#ifndef IFBRANCH_H
#define IFBRANCH_H

#include "CustomBranch.h"
class IFBranch : public CustomBranch {
public:
    IFBranch(Compiler* compiler);
    virtual ~IFBranch();
    
    void setExpressionBranch(std::shared_ptr<Branch> branch);
    void setBodyBranch(std::shared_ptr<Branch> branch);
    
    std::shared_ptr<Branch> getExpressionBranch();
    std::shared_ptr<Branch> getBodyBranch();
private:

};

#endif /* IFBRANCH_H */

