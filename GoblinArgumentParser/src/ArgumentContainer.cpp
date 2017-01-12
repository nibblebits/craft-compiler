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
 * File:   ArgumentContainer.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 June 2016, 00:55
 * 
 * Description: A container for the arguments 
 */

#include "ArgumentContainer.h"
#include "Argument.h"

ArgumentContainer::ArgumentContainer()
{

}

ArgumentContainer::ArgumentContainer(std::vector<Argument> arguments)
{
    this->arguments = arguments;
}

ArgumentContainer::~ArgumentContainer()
{
}

Argument* ArgumentContainer::findArgument(std::string name)
{
    for (int i = 0; i < this->arguments.size(); i++)
    {
        if (arguments[i].name == name)
            return &arguments[i];
    }

    return NULL;
}

std::string ArgumentContainer::getArgumentValue(std::string name)
{
    for (Argument arg : this->arguments)
    {
        if (arg.name == name)
            return arg.value;
    }

    return "";
}

bool ArgumentContainer::hasArgument(std::string name)
{
    for (Argument arg : this->arguments)
    {
        if (arg.name == name)
            return true;
    }

    return false;
}

std::vector<Argument> ArgumentContainer::getArguments()
{
    return this->arguments;
}