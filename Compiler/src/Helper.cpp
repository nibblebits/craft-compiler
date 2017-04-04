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
 * File:   Helper.cpp
 * Author: Daniel McCarthy
 *
 * Created on 30 May 2016, 04:02
 * 
 * Description: A helper class that contains common functions
 */

#include "Helper.h"

std::vector<std::string> Helper::split(std::string str, char delimiter)
{
    std::vector<std::string> strings;
    std::size_t l_pos = 0;
    std::size_t pos = 0;
    while (true)
    {
        pos = str.find(delimiter, l_pos);
        std::string f = str.substr(l_pos, pos - l_pos);
        strings.push_back(f);
        l_pos = pos + 1;
        if (pos == std::string::npos)
            break;
    }

    return strings;
}

std::string Helper::str_remove(std::string str, char remove)
{
    std::string new_str = "";
    for (char c : str)
    {
        if (c != remove)
        {
            new_str += c;
        }
    }

    return new_str;
}

std::string Helper::str_remove(std::string str, int index)
{
    std::string new_str = "";
    for (int i = 0; i < str.size(); i++)
    {
        if (i != index)
        {
            new_str += str[i];
        }
    }

    return new_str;
}

std::string Helper::str_replace(std::string str, std::string target, std::string replacement)
{
    std::string new_str = "";
    std::string::size_type n = 0;
    std::string::size_type prev_n = 0;
    while((n = str.find(target, n)) != std::string::npos)
    {
        new_str += str.substr(prev_n, (n-prev_n)) + replacement;
        prev_n = n+1;
        n += target.size();
    }
    
    if (n == 0)
    {
        // No replacement was found set the new string to the current one
        new_str = str;
    }
    else
    {
        // Ok we need to add on the final part
        new_str += str.substr(prev_n, (str.size()-prev_n));
    }
    
    return new_str;
}