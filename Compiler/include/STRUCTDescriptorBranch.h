/*
    Craft compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   STRUCTDescriptorBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 23 February 2017, 18:20
 */

#ifndef STRUCTDESCRIPTORBRANCH_H
#define STRUCTDESCRIPTORBRANCH_H

#include "CustomBranch.h"

class EXPORT STRUCTDescriptorBranch : public CustomBranch
{
public:
    STRUCTDescriptorBranch(Compiler* compiler);
    STRUCTDescriptorBranch(Compiler* compiler, std::string branch_type, std::string branch_value);
    virtual ~STRUCTDescriptorBranch();

    void setStructNameBranch(std::shared_ptr<Branch> name_branch);
    std::shared_ptr<Branch> getStructNameBranch();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();
private:

};

#endif /* STRUCTDESCRIPTORBRANCH_H */

