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
 * File:   common.h
 * Author: Daniel McCarthy
 *
 * Created on 14 September 2016, 01:26
 * 
 * Description: Common functions are held here
 */

#ifndef COMMON_H
#define COMMON_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include "Token.h"
#include "Branch.h"
#include "Stream.h"
#include "def.h"

std::string EXPORT LoadFile(std::string filename);
void EXPORT debug_output_tokens(std::vector<std::shared_ptr<Token>> tokens);
void EXPORT debug_output_branch(std::shared_ptr<Branch> branch, int no_tabs = 0);
void EXPORT WriteFile(std::string filename, Stream* stream);

#endif /* COMMON_H */
