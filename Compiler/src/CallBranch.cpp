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
 * File:   CallBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 21 June 2016, 21:02
 * 
 * Description: The branch object for a "CALL" branch.
 */

#include "CallBranch.h"

CallBranch::CallBranch(Compiler* compiler) : CustomBranch(compiler, "CALL", "") {
}

CallBranch::~CallBranch() {
}


std::shared_ptr<Branch> CallBranch::getFunctionNameBranch()
{
    std::shared_ptr<Branch> name_branch = this->getChildren()[0]->getChildren()[0];
    return name_branch;
}

std::vector<std::shared_ptr<Branch>> CallBranch::getFunctionArgumentBranches()
{
    std::vector<std::string> search_for;
    search_for.push_back("MATH_E");
    search_for.push_back("identifier");
    search_for.push_back("number");
    
    std::vector<std::shared_ptr<Branch>> argument_children = this->getCompiler()->getASTAssistant()->findAllChildrenOfType(this->getptr()->getChildren()[1], search_for);
    return argument_children;
}
    