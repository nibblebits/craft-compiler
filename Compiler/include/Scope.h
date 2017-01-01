/*
    Craft Compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   Scope.h
 * Author: Daniel McCarthy
 *
 * Created on 26 July 2016, 00:09
 */

#ifndef SCOPE_H
#define SCOPE_H

#include <cstddef>
#include <memory>
#include <vector>
#include "Branch.h"
#include "structs.h"

class Scope : public std::enable_shared_from_this<Scope>
{
public:
    Scope(int memory_pos);
    virtual ~Scope();
    static std::shared_ptr<Scope> newInstance(int memory_pos);
    void setParentScope(std::shared_ptr<Scope> scope);
    void addChildScope(std::shared_ptr<Scope> scope);
    bool hasChildScope(std::shared_ptr<Scope> scope);
    std::shared_ptr<Scope> getParentScope();
    std::vector<std::shared_ptr<Scope>> getChildScopes();
    bool hasParent();
    bool hasChild();
    std::shared_ptr<struct variable> registerVariableFromBranch(std::shared_ptr<Branch> branch);
    std::shared_ptr<struct variable> getVariableInThisScope(std::string name);
    std::shared_ptr<struct variable> getVariable(std::string name);
    int getMemoryPosition();
    size_t getVariablesSize();
    size_t getVariablesSizeForThisScope();
    std::shared_ptr<Scope> getptr();
private:
    int getNextVariableMemoryPosition();
    int memory_pos;
    std::shared_ptr<Scope> parent_scope;
    std::vector<std::shared_ptr<Scope>> child_scopes;
    std::shared_ptr<struct variable> createVariableFromBranch(std::shared_ptr<Branch> branch);
    std::vector<std::shared_ptr<struct variable>> variables;
};

#endif /* SCOPE_H */

