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
 * File:   TypeChecker.cpp
 * Author: Daniel McCarthy
 *
 * Created on 04 June 2016, 18:32
 * 
 * Description: The type checker is responsible for checking the tree to make sure it is valid.
 * For example the type checker will check variables are defined in the tree before assignments are defined for that particular variable.
 */

#include "TypeChecker.h"
#include "Compiler.h"

TypeChecker::TypeChecker(Compiler* compiler) : CompilerEntity(compiler)
{
    this->astAssistant = compiler->getASTAssistant();
}

TypeChecker::~TypeChecker()
{
}

void TypeChecker::setTree(std::shared_ptr<Tree> tree)
{
    this->tree = tree;
}

void TypeChecker::validate()
{
    if (this->tree == NULL)
    {
        throw TypeCheckerException("No tree has been set");
    }
    std::shared_ptr<Branch> root = this->tree->root;


}

void TypeChecker::Check(std::shared_ptr<Branch> branch)
{
    // To be written another time :)
    
    return;
    
}

bool TypeChecker::isEntityRegistered(std::string name)
{
    std::vector<struct entity>* entities = &this->scopes.top();
    if (isEntityInVector(entities, name))
        return true;
    
    // Its not in the scope so lets check the global scope
    if (isEntityInVector(this->global_scope, name))
        return true;

    return false;
}


bool TypeChecker::isEntityInVector(std::vector<struct entity>* vector, std::string name)
{
    for (std::vector<struct entity>::iterator it = vector->begin(); it < vector->end(); it++)
    {
        struct entity en = *it;
        if (en.name == name)
            return true;
    }
    
    return false;
}

void TypeChecker::registerEntity(std::string type, std::string name)
{
    std::vector<struct entity>* entities = &this->scopes.top();
    struct entity en;
    en.type = type;
    en.name = name;
    entities->push_back(en);
}

void TypeChecker::throwAlreadyDeclaredException(std::shared_ptr<Branch> branch)
{
    if (branch->getBranchType() != BRANCH_TYPE_TOKEN)
    {
        throw TypeCheckerException("An issue with throwing a declared exception has occurred");
    }

    std::shared_ptr<Token> token = std::dynamic_pointer_cast<Token>(branch);
    throw TypeCheckerException(token->getPosition(), "the entity '" + token->getValue() + "' has already been declared.");
}

void TypeChecker::throwUndeclaredException(std::shared_ptr<Branch> branch)
{
    if (branch->getBranchType() != BRANCH_TYPE_TOKEN)
    {
        throw TypeCheckerException("An issue with throwing an undeclared exception has occurred");
    }

    std::shared_ptr<Token> token = std::dynamic_pointer_cast<Token>(branch);
    throw TypeCheckerException(token->getPosition(), "the entity '" + token->getValue() + "' has not been declared.");
}