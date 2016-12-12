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
 * File:   TreeImprover.h
 * Author: Daniel McCarthy
 *
 * Created on 10 December 2016, 12:52
 */

#ifndef TREEIMPROVER_H
#define TREEIMPROVER_H

#include <memory>
#include "CompilerEntity.h"

class Tree;
class FuncBranch;
class Branch;
class VarIdentifierBranch;
class BODYBranch;
class IFBranch;
class FORBranch;
class PTRBranch;

class EXPORT TreeImprover : public CompilerEntity
{
public:
    TreeImprover(Compiler* compiler);
    virtual ~TreeImprover();

    void setTree(std::shared_ptr<Tree> tree);
    void improve();
private:
    void improve_top();
    void improve_branch(std::shared_ptr<Branch> branch);
    void improve_func(std::shared_ptr<FuncBranch> func_branch);
    void improve_func_arguments(std::shared_ptr<Branch> func_args_branch);
    void improve_body(std::shared_ptr<BODYBranch> body_branch);
    void improve_expression(std::shared_ptr<Branch> expression_branch);
    void improve_var_iden(std::shared_ptr<VarIdentifierBranch> var_iden_branch);
    void improve_if(std::shared_ptr<IFBranch> if_branch);
    void improve_for(std::shared_ptr<FORBranch> for_branch);
    void improve_ptr(std::shared_ptr<PTRBranch> ptr_branch);

    std::shared_ptr<Tree> tree;
    VARIABLE_TYPE current_var_type;

};

#endif /* TREEIMPROVER_H */

