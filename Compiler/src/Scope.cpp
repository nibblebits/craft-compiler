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
 * File:   Scope.cpp
 * Author: Daniel McCarthy
 *
 * Created on 26 July 2016, 00:09
 * 
 * Description: 
 */

#include "Scope.h"
#include "branches.h"

Scope::Scope(int memory_pos)
{
    this->memory_pos = memory_pos;
    this->parent_scope = NULL;
}

Scope::~Scope()
{
}

std::shared_ptr<Scope> Scope::newInstance(int memory_pos)
{
    return std::shared_ptr<Scope>(new Scope(memory_pos));
}

void Scope::setParentScope(std::shared_ptr<Scope> scope)
{
    this->parent_scope = scope;
    if (this->parent_scope != NULL)
        this->parent_scope->addChildScope(this->getptr());
}

void Scope::addChildScope(std::shared_ptr<Scope> scope)
{
    if (!hasChildScope(scope))
    {
        this->child_scopes.push_back(scope);
    }
}

bool Scope::hasChildScope(std::shared_ptr<Scope> scope)
{
    for (std::shared_ptr<Scope> _scope : this->child_scopes)
    {
        if (scope == _scope)
        {
            return true;
        }
    }

    return false;
}

std::shared_ptr<Scope> Scope::getParentScope()
{
    return this->parent_scope;
}

std::vector<std::shared_ptr<Scope>> Scope::getChildScopes()
{
    return this->child_scopes;
}

int Scope::getMemoryPosition()
{
    return this->memory_pos;
}

size_t Scope::getVariablesSize()
{
    size_t t_size = this->getVariablesSizeForThisScope();
    if (this->hasParent())
    {
        t_size += this->getParentScope()->getVariablesSize();
    }

    if (this->hasChild())
    {
        for (std::shared_ptr<Scope> scope : this->child_scopes)
        {
            t_size += scope->getVariablesSize();
        }
    }

    return t_size;
}

size_t Scope::getVariablesSizeForThisScope()
{
    size_t t_size = 0;
    for (std::shared_ptr<struct variable> var : this->variables)
    {
        t_size += var->size;
    }

    return t_size;
}

bool Scope::hasParent()
{
    return this->parent_scope != NULL;
}

bool Scope::hasChild()
{
    return this->child_scopes.size() != 0;
}

int Scope::getNextVariableMemoryPosition()
{
    int mem_pos = 0;
    if (this->hasParent())
    {
        mem_pos = this->getParentScope()->getNextVariableMemoryPosition();
    }


    if (!this->variables.empty())
    {
        std::shared_ptr<struct variable> last_var = this->variables.back();
        mem_pos += last_var->mem_pos + last_var->size;
    }

    return mem_pos;
}

std::shared_ptr<struct variable> Scope::registerVariableFromBranch(std::shared_ptr<Branch> branch)
{
    if (branch->getType() != "V_DEF")
    {
        throw Exception("The branch: " + branch->getType() + " cannot be converted to a scope variable");
    }


    std::shared_ptr<struct variable> var = this->createVariableFromBranch(branch);
    var->mem_pos = this->getNextVariableMemoryPosition();

    this->variables.push_back(var);

    return var;
}

std::shared_ptr<struct variable> Scope::createVariableFromBranch(std::shared_ptr<Branch> branch)
{
    std::shared_ptr<struct variable> variable = NULL;
    std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
    std::shared_ptr<Token> name_branch = std::dynamic_pointer_cast<Token>(vdef_branch->getKeywordBranch());
    std::shared_ptr<Token> type_branch = std::dynamic_pointer_cast<Token>(vdef_branch->getNameBranch());
    std::string type_value = type_branch->getValue();

    if (type_branch->getType() == "keyword")
    {
        int size_per_elem = Compiler::getDataTypeSize(type_value);
        variable = std::shared_ptr<struct variable > (new struct variable);
        variable->size = size_per_elem;
        variable->is_array = false;
    }

    variable->name = name_branch->getValue();
    variable->type = type_value;
    variable->scope = this->getptr();

    return variable;
}

std::shared_ptr<struct variable> Scope::getVariableInThisScope(std::string name)
{
    for (std::shared_ptr<struct variable> var : this->variables)
    {
        if (var->name == name)
            return var;
    }

    return NULL;
}

/* Gets a variable from our own scope or our parents, parents, parents and so on...*/
std::shared_ptr<struct variable> Scope::getVariable(std::string name)
{
    // Search using our own variables first
    std::shared_ptr<struct variable> var = this->getVariableInThisScope(name);
    if (var != NULL)
    {
        return var;
    }

    // No match? Search our parent
    if (this->hasParent())
    {
        var = this->getParentScope()->getVariable(name);
        return var;
    }
    return NULL;
}

std::shared_ptr<Scope> Scope::getptr()
{
    return shared_from_this();
}