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

class Branch
{
public:
    Branch(std::string type, std::string value);
    virtual ~Branch();

    void addChild(std::shared_ptr<Branch> branch);
    std::vector<std::shared_ptr<Branch>> getChildren();
    std::string getType();
    std::string getValue();
private:
    std::string type;
    std::string value;
    std::vector<std::shared_ptr<Branch>> children;
};

#endif /* BRANCH_H */

