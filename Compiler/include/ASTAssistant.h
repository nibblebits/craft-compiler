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
 * File:   ASTAssistant.h
 * Author: Daniel McCarthy
 *
 * Created on 17 June 2016, 00:50
 */

#ifndef ASTASSISTANT_H
#define ASTASSISTANT_H

#include <vector>
#include "Branch.h"
#include "CompilerEntity.h"

class ASTAssistant : public CompilerEntity
{
public:
    ASTAssistant(Compiler* compiler);
    virtual ~ASTAssistant();
    std::vector<std::shared_ptr<Branch>> findAllChildrenOfType(std::shared_ptr<Branch> root, std::string branch_type);
    std::vector<std::shared_ptr<Branch>> findAllChildrenOfType(std::shared_ptr<Branch> root, std::vector<std::string> branch_types);
private:

};

#endif /* ASTASSISTANT_H */

