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
 * File:   GoblinArgumentParser.h
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 23:56
 * 
 */

#ifndef GOBLINARGUMENTPARSER_H
#define GOBLINARGUMENTPARSER_H

#include <vector>
#include <string>
#include "Argument.h"
#include "ArgumentContainer.h"
#include "GoblinArgumentException.h"

#define DLL_EXPORT __declspec(dllexport)

ArgumentContainer DLL_EXPORT GoblinArgumentParser_GetArguments(int argc, char** argv);
#endif /* GOBLINARGUMENTPARSER_H */

