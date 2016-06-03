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
 * File:   ParserRuleRequirement.h
 * Author: Daniel McCarthy
 *
 * Created on 30 May 2016, 02:39
 */

#ifndef PARSERRULEREQUIREMENT_H
#define PARSERRULEREQUIREMENT_H

#include <string>
#include <vector>
class ParserRuleRequirement {
public:
    ParserRuleRequirement(std::string _class);
    virtual ~ParserRuleRequirement();
    
    void allow(std::string value);
    bool allowed(std::string value);
    void excludeFromTree(bool exclude);
    bool hasValueRequirements();
    bool excluded();
    std::string getClassName();
    std::vector<std::string> getAllowedValues();
private:
    std::string _class;
    std::vector<std::string> allowed_classes;
    bool exclude_from_tree;

};

#endif /* PARSERRULEREQUIREMENT_H */

