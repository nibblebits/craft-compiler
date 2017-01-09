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
 * File:   MacroDefineBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 09 January 2017, 19:54
 */

#ifndef MACRODEFINEBRANCH_H
#define MACRODEFINEBRANCH_H

#include "CustomBranch.h"

class MacroDefineBranch : public CustomBranch
{
public:
    MacroDefineBranch(Compiler* compiler);
    virtual ~MacroDefineBranch();

    void setDefinitionNameBranch(std::shared_ptr<Branch> name_branch);
    void setDefinitionValueBranch(std::shared_ptr<Branch> value_branch);

    std::shared_ptr<Branch> getDefinitionNameBranch();
    std::shared_ptr<Branch> getDefinitionValueBranch();
    
    bool hasDefinitionValueBranch();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();
private:

};

#endif /* MACRODEFINEBRANCH_H */

