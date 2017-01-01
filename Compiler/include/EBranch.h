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
 * File:   EBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 28 June 2016, 03:30
 */

#ifndef EBRANCH_H
#define EBRANCH_H

#include "CustomBranch.h"

class EXPORT EBranch : public CustomBranch
{
public:
    EBranch(Compiler* compiler, std::string value);
    virtual ~EBranch();
    void iterate_expressions(std::function<void(std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) > func);
    void iterate_expressions(std::function<void(std::shared_ptr<Branch> left_branch) > left_func, std::function<void(std::shared_ptr<Branch> right_branch) > right_func);
    virtual void validity_check();
    virtual void rebuild();
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();
private:

};

#endif /* EBRANCH_H */

