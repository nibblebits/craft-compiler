/*
    Goblin Argument Parser v1.0 - A parser for translating program arguments.
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
 * File:   GoblinArgumentParser.cpp
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 23:58
 */

#include "GoblinArgumentParser.h"

ArgumentContainer DLL_EXPORT GoblinArgumentParser_GetArguments(int argc, char** argv)
{
    std::vector<Argument> arguments;
    for (int i = 1; i < argc; i++)
    {
        Argument argument;
        std::string argument_name = std::string(argv[i]);
        argument_name.erase(argument_name.begin());
        argument.name = argument_name;
       
        if (i + 1 < argc)
        {
            std::string argument_value = std::string(argv[i+1]);
            if (argument_value.find("-") == std::string::npos)
            {
                argument.value = argument_value;
                i++;
           }
        }
        arguments.push_back(argument);
    }
    
    return ArgumentContainer(arguments);
}