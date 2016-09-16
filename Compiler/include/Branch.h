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
 * File:   Branch.h
 * Author: Daniel McCarthy
 *
 * Created on 29 May 2016, 20:41
 */

#ifndef BRANCH_H
#define BRANCH_H

#include <cstddef>
#include <string>
#include <vector>
#include <memory>

#include "Exception.h"
#include "def.h"

#define BRANCH_TYPE_BRANCH 0
#define BRANCH_TYPE_TOKEN 1

class EXPORT Branch : public std::enable_shared_from_this<Branch>
{
public:
    Branch(std::string type, std::string value);
    virtual ~Branch();

    void addChild(std::shared_ptr<Branch> branch);
    void replaceChild(std::shared_ptr<Branch> child, std::shared_ptr<Branch> new_branch);
    virtual void removeChild(std::shared_ptr<Branch> child);
    void exclude(bool excluded);
    bool excluded();
    void setParent(std::shared_ptr<Branch> branch);
    std::shared_ptr<Branch> getFirstChild();
    std::shared_ptr<Branch> getSecondChild();
    std::shared_ptr<Branch> getThirdChild();
    std::shared_ptr<Branch> getFourthChild();
    std::vector<std::shared_ptr<Branch>> getChildren();
    bool hasChild(std::shared_ptr<Branch> branch);
    std::shared_ptr<Branch> getParent();
    bool hasParent();
    std::shared_ptr<Branch> lookUpTreeUntilParentTypeFound(std::string parent_type_to_find);
    std::shared_ptr<Branch> lookDownTreeUntilFirstChildOfType(std::string type);
    std::shared_ptr<Branch> lookDownTreeUntilLastChildOfType(std::string type);
    std::string getType();
    std::string getValue();

    std::shared_ptr<Branch> getptr();
    virtual int getBranchType();
private:
    std::string type;
    std::string value;
    std::vector<std::shared_ptr<Branch>> children;
    std::shared_ptr<Branch> parent;
    bool excluded_from_tree;
};

#endif /* BRANCH_H */

