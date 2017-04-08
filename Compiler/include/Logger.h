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
 * File:   Logger.h
 * Author: Daniel McCarthy
 *
 * Created on 07 September 2016, 21:44
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <memory>
#include "def.h"

class CustomBranch;
class Branch;
class EXPORT Logger {
public:
    Logger();
    virtual ~Logger();

    void error(std::string message, std::shared_ptr<Branch> bad_branch=NULL);
    void warn(std::string message, std::shared_ptr<Branch> bad_branch=NULL);

    std::vector<std::string> getLog();
    std::string getLogAsString();
    bool hasAnError();
    bool hasErrorOrWarning();
private:
    int total_errors;
    std::vector<std::string> log;
};

#endif /* LOGGER_H */

