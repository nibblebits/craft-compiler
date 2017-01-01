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
 * File:   SemanticValidator.h
 * Author: Daniel McCarthy
 *
 * Created on 10 December 2016, 00:07
 */

#ifndef SEMANTICVALIDATOR_H
#define SEMANTICVALIDATOR_H

#include <memory>
#include "CompilerEntity.h"
class Tree;
class EXPORT SemanticValidator : public CompilerEntity {
public:
    SemanticValidator(Compiler* compiler);
    virtual ~SemanticValidator();
    
    void setTree(std::shared_ptr<Tree> tree);
    void validate();
private:
    std::shared_ptr<Tree> tree;
};

#endif /* SEMANTICVALIDATOR_H */

