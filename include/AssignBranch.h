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
 * File:   AssignBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 28 June 2016, 00:01
 */

#ifndef ASSIGNBRANCH_H
#define ASSIGNBRANCH_H

#include "CustomBranch.h"
class AssignBranch : public CustomBranch {
public:
    AssignBranch(Compiler* compiler);
    virtual ~AssignBranch();
    std::shared_ptr<Branch> getVariableToAssignBranch();
    std::shared_ptr<Branch> getAssignmentTypeBranch();
    std::shared_ptr<Branch> getValueBranch();
    std::shared_ptr<Branch> getArrayIndexRootBranch();
    bool isVariableToAssignInArray();
private:

};

#endif /* ASSIGNBRANCH_H */

