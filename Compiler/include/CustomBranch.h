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
 * File:   CustomBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 21 June 2016, 21:21
 */

#ifndef CUSTOMBRANCH_H
#define CUSTOMBRANCH_H

#include <map>
#include "Compiler.h"
#include "Branch.h"
#include "CharPos.h"

typedef std::map<std::string, std::shared_ptr<Branch>>::iterator map_it;

class EXPORT CustomBranch : public Branch
{
public:
    CustomBranch(Compiler* compiler, std::string name, std::string value);
    virtual ~CustomBranch();

    void registerBranch(std::string name, std::shared_ptr<Branch> branch);
    std::shared_ptr<Branch> getRegisteredBranchByName(std::string name);
    bool isBranchRegistered(std::string name);

    void output_registered_branches();
    Compiler* getCompiler();
    
    // In the future all branches will be required a position so this wont be required anymore.
    CharPos getClosestPosition();

    virtual void replaceChild(std::shared_ptr<Branch> child, std::shared_ptr<Branch> new_branch);

    virtual std::shared_ptr<Branch> clone();
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch) = 0;
    virtual std::shared_ptr<Branch> create_clone() = 0;

private:
    std::map<std::string, std::shared_ptr<Branch>> registered_branches;

    Compiler* compiler;
};

#endif /* CUSTOMBRANCH_H */

