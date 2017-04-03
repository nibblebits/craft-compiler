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
 * File:   Logger.cpp
 * Author: Daniel McCarthy
 *
 * Created on 07 September 2016, 21:44
 * 
 * Description: Provides the ability to log and view logs
 */

#include <vector>
#include "Logger.h"
#include "CustomBranch.h"

Logger::Logger()
{
    this->total_errors = 0;
}

Logger::~Logger()
{
}

void Logger::error(std::string message, std::shared_ptr<Branch> bad_branch)
{
    CharPos position;
    std::shared_ptr<CustomBranch> c_bad_branch = std::dynamic_pointer_cast<CustomBranch>(bad_branch);
    std::shared_ptr<Token> bad_token = std::dynamic_pointer_cast<Token>(bad_branch);
    message = "error: " + message;
    if (c_bad_branch != NULL)
    {
        // Adjust the message with the closest char position we can find (if any)
        try
        {
            position = c_bad_branch->getClosestPosition();
        }
        catch (Exception& ex)
        {
        }
    }
    else if (bad_token != NULL)
    {
        position = bad_token->getPosition();
    }

    if (c_bad_branch != NULL
            || bad_token != NULL)
    {
        message += " in file \"" + position.filename + "\" on line " + std::to_string(position.line_no) + ", col:" + std::to_string(position.col_pos);
    }

    this->total_errors++;
    this->log.push_back(message);
}

void Logger::warn(std::string message, std::shared_ptr<Branch> bad_branch)
{
    CharPos position;
    std::shared_ptr<CustomBranch> c_bad_branch = std::dynamic_pointer_cast<CustomBranch>(bad_branch);
    std::shared_ptr<Token> bad_token = std::dynamic_pointer_cast<Token>(bad_branch);
    message = "warning: " + message;
    if (c_bad_branch != NULL)
    {
        // Adjust the message with the closest char position we can find (if any)
        try
        {
            position = c_bad_branch->getClosestPosition();
        }
        catch (Exception& ex)
        {
        }
    }
    else if (bad_token != NULL)
    {
        position = bad_token->getPosition();
    }

    if (c_bad_branch != NULL
            || bad_token != NULL)
    {
        message += " in file \"" + position.filename + "\" on line " + std::to_string(position.line_no) + ", col:" + std::to_string(position.col_pos);
    }

    this->log.push_back(message);
}

std::vector<std::string> Logger::getLog()
{
    return this->log;
}

bool Logger::hasAnError()
{
    return this->total_errors != 0;
}

bool Logger::hasErrorOrWarning()
{
    return !this->log.empty();
}