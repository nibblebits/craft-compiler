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
class CustomBranch : public Branch {
public:
    CustomBranch(Compiler* compiler, std::string name, std::string value);
    virtual ~CustomBranch();
    
    void registerBranch(std::string name, std::shared_ptr<Branch> branch);
    std::shared_ptr<Branch> getRegisteredBranchByName(std::string name);
    Compiler* getCompiler();
private:
    std::map<std::string, std::shared_ptr<Branch>> registered_branches;
    Compiler* compiler;
};

#endif /* CUSTOMBRANCH_H */

