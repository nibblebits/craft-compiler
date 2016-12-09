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
 * File:   CustomBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 21 June 2016, 21:21
 * 
 * Description: Custom branches are those who provide particular methods for a particular branch.
 * Any branch wishing to be a custom branch need only to extend this class.
 */

#include "CustomBranch.h"
#include "ScopeBranch.h"

CustomBranch::CustomBranch(Compiler* compiler, std::string name, std::string value) : Branch(name, value)
{
    this->compiler = compiler;
}

CustomBranch::~CustomBranch()
{

}

void CustomBranch::registerBranch(std::string name, std::shared_ptr<Branch> branch)
{
    if (branch != NULL)
    {
        // If the branch getting registered is currently not a child then make it one
        if (!hasChild(branch))
        {
            addChild(branch);
        }
    }
    this->registered_branches[name] = branch;
}

std::shared_ptr<Branch> CustomBranch::getRegisteredBranchByName(std::string name)
{
    if (!isBranchRegistered(name))
        return NULL;

    return this->registered_branches[name];
}

bool CustomBranch::isBranchRegistered(std::string name)
{
    std::map<std::string, std::shared_ptr < Branch>>::const_iterator it = this->registered_branches.find(name);
    return it != this->registered_branches.end();
}

void CustomBranch::output_registered_branches()
{
    debug_output_branch(this->getptr());
    for (map_it iterator = this->registered_branches.begin();
            iterator != this->registered_branches.end(); iterator++)
    {
        std::cout << "Registered branch: " << iterator->first << "; ptr: " << iterator->second << std::endl;
    }
}

Compiler* CustomBranch::getCompiler()
{
    return this->compiler;
}

// Scopes must not be cloned.
std::shared_ptr<Branch> CustomBranch::clone()
{
    std::shared_ptr<Branch> cloned_branch = create_clone();
    imp_clone(cloned_branch);
    return cloned_branch;
}