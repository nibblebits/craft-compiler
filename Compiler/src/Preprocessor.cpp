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
 * File:   Preprocessor.cpp
 * Author: Daniel McCarthy
 *
 * Created on 08 January 2017, 01:58
 * 
 * Description: 
 */

#include "Preprocessor.h"
#include "Tree.h"
#include "branches.h"

Preprocessor::Preprocessor(Compiler* compiler) : CompilerEntity(compiler)
{
    this->definitions.push_back("TEST");
}

Preprocessor::~Preprocessor()
{
}

void Preprocessor::setTree(std::shared_ptr<Tree> tree)
{
    this->tree = tree;
}

void Preprocessor::process()
{
    std::shared_ptr<Branch> root = this->tree->root;
    // Lets get all the children that are macros and skip everything else
    for (std::shared_ptr<Branch> child : root->getChildren())
    {
        if (is_macro(child->getType()))
        {
            process_macro(child);
        }
    }
}

bool Preprocessor::is_macro(std::string macro_name)
{
    return (macro_name == "MACRO_IFDEF");
}

bool Preprocessor::is_definition_registered(std::string definition_name)
{
    for (std::string definition : this->definitions)
    {
        if (definition == definition_name)
        {
            return true;
        }
    }
    
    return false;
}
void Preprocessor::process_macro(std::shared_ptr<Branch> macro)
{
    if (macro->getType() == "MACRO_IFDEF")
    {
        std::shared_ptr<MacroIfDefBranch> macro_ifdef_branch = std::dynamic_pointer_cast<MacroIfDefBranch>(macro);
        process_macro_ifdef(macro_ifdef_branch);
    }
}

void Preprocessor::process_macro_ifdef(std::shared_ptr<MacroIfDefBranch> macro_ifdef_branch)
{
    std::shared_ptr<Branch> req_branch = macro_ifdef_branch->getRequirementBranch();
    std::shared_ptr<BODYBranch> body_branch = macro_ifdef_branch->getBodyBranch();
    
    if (is_definition_registered(req_branch->getValue()))
    {
        // The definition is registered so replace this macro with the body branch
        macro_ifdef_branch->replaceSelf(body_branch);
        body_branch->replaceWithChildren();
    }
    else
    {
        // All were false so lets delete ourself and all children associated with us
        macro_ifdef_branch->removeSelf();
    }
}