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
 * File:   GlobalBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 11 January 2017, 02:30
 */

#ifndef GLOBALBRANCH_H
#define GLOBALBRANCH_H

#include "CustomBranch.h"
class GlobalBranch : public CustomBranch
{
public:
    GlobalBranch(Compiler* compiler);
    virtual ~GlobalBranch();

    void setLabelNameBranch(std::shared_ptr<Branch> branch);
    std::shared_ptr<Branch> getLabelNameBranch();
    
    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();
private:

};

#endif /* GLOBALBRANCH_H */

