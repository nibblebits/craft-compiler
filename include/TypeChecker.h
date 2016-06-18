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
 * File:   TypeChecker.h
 * Author: Daniel McCarthy
 *
 * Created on 04 June 2016, 18:32
 */

#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <memory>
#include <stack>
#include "Tree.h"
#include "TypeCheckerException.h"
#include "Token.h"
#include "ASTAssistant.h"
#include "CompilerEntity.h"
class TypeChecker : public CompilerEntity
{
public:
    TypeChecker(Compiler* compiler);
    virtual ~TypeChecker();

    void setTree(std::shared_ptr<Tree> tree);
    void validate();
private:
    /* This structure will need changing should this language become object orientated */
    struct entity {
        std::string type;
        std::string name;
    };
    
    std::stack<std::vector<struct entity>> scopes;
    std::shared_ptr<Tree> tree;
    ASTAssistant* astAssistant;  
    std::vector<struct entity>* global_scope;
    void Check(std::shared_ptr<Branch> branch);
    bool isEntityRegistered(std::string name);
    bool isEntityInVector(std::vector<struct entity>* vector, std::string name);
    void registerEntity(std::string type, std::string name);
    
    void throwAlreadyDeclaredException(std::shared_ptr<Branch> branch);
    void throwUndeclaredException(std::shared_ptr<Branch> branch);
};

#endif /* TYPECHECKER_H */

