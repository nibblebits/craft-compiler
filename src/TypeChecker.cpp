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

TypeChecker::TypeChecker()
{
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

    // Push a blank variable vector to represent the global scope
    std::vector<struct variable> variables;
    this->scopes.push(variables);
    this->global_scope = &this->scopes.top();
    Check(root);
}

void TypeChecker::Check(std::shared_ptr<Branch> branch)
{
    if (branch != NULL)
    {
        std::string branch_type = branch->getType();
        std::vector<std::shared_ptr < Branch>> children = branch->getChildren();

        if (branch_type == "V_DEF")
        {
            std::shared_ptr<Branch> var_name_branch = children[1]->getChildren()[0];
            std::string var_type = children[0]->getValue();
            std::string var_name = var_name_branch->getValue();
            if (isVariableRegistered(var_name))
            {
                throwAlreadyDeclaredException(var_name_branch);
            }
            registerVariable(var_type, var_name);
        }
        else if (branch_type == "ASSIGN")
        {
            // Not yet complete
            std::shared_ptr<Branch> variable_name_branch = children[0]->getChildren()[0];
            std::shared_ptr<Branch> variable_value_branch = children[2]->getChildren()[0];
            std::string variable_name = variable_name_branch->getValue();
            std::string variable_value = variable_value_branch->getValue();

            if (!isVariableRegistered(variable_name))
            {
                throwUndeclaredException(variable_name_branch);
            }
            if (variable_value_branch->getType() == "identifier")
            {
                if (!isVariableRegistered(variable_value))
                {
                    throwUndeclaredException(variable_value_branch);
                }
            }
        }
        else if (branch_type == "SCOPE")
        {
            // Push a new variable scope as we are in a new scope now
            std::vector<struct variable> variables;
            this->scopes.push(variables);
        }

        // Check its children.
        for (std::shared_ptr<Branch> child_branch : children)
        {
            Check(child_branch);
        }
        
        if (branch_type == "SCOPE")
        {
            // At this point all children would have assigned/used variables of the scope and now we need to pop it off
            this->scopes.pop();
        }
    }
}

bool TypeChecker::isVariableRegistered(std::string name)
{
    std::vector<struct variable>* variables = &this->scopes.top();
    if (isVariableInVector(variables, name))
        return true;
    
    // Its not in the scope so lets check the global scope
    if (isVariableInVector(this->global_scope, name))
        return true;

    return false;
}


bool TypeChecker::isVariableInVector(std::vector<struct variable>* vector, std::string name)
{
    for (std::vector<struct variable>::iterator it = vector->begin(); it < vector->end(); it++)
    {
        struct variable var = *it;
        if (var.name == name)
            return true;
    }
    
    return false;
}

void TypeChecker::registerVariable(std::string type, std::string name)
{
    std::vector<struct variable>* variables = &this->scopes.top();
    struct variable var;
    var.type = type;
    var.name = name;
    variables->push_back(var);
}

void TypeChecker::throwAlreadyDeclaredException(std::shared_ptr<Branch> branch)
{
    if (branch->getBranchType() != BRANCH_TYPE_TOKEN)
    {
        throw TypeCheckerException("An issue with throwing a declared exception has occurred");
    }

    std::shared_ptr<Token> token = std::dynamic_pointer_cast<Token>(branch);
    throw TypeCheckerException(token->getPosition(), "the variable '" + token->getValue() + "' has already been declared.");
}

void TypeChecker::throwUndeclaredException(std::shared_ptr<Branch> branch)
{
    if (branch->getBranchType() != BRANCH_TYPE_TOKEN)
    {
        throw TypeCheckerException("An issue with throwing an undeclared exception has occurred");
    }

    std::shared_ptr<Token> token = std::dynamic_pointer_cast<Token>(branch);
    throw TypeCheckerException(token->getPosition(), "the variable '" + token->getValue() + "' has not been declared.");
}