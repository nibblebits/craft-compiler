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
 * File:   OffsetableBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 01 January 2017, 21:32
 */

#ifndef OFFSETABLEBRANCH_H
#define OFFSETABLEBRANCH_H

#include "CustomBranch.h"
class OffsetableBranch : public CustomBranch
{
public:
    OffsetableBranch(Compiler* compiler, std::string type, std::string value);
    virtual ~OffsetableBranch();

    void setOffset(int offset);
    int getOffset();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();


private:
    int offset;
};

#endif /* OFFSETABLEBRANCH_H */

