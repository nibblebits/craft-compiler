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
 * File:   common.h
 * Author: Daniel McCarthy
 *
 * Created on 14 September 2016, 01:26
 * 
 * Description: Common functions are held here
 */

#include "common.h"

using namespace std;

#ifdef DEBUG_MODE

void EXPORT debug_output_tokens(std::vector<std::shared_ptr<Token>> tokens)
{
    std::cout << "DEBUG TOKEN OUTPUT" << std::endl;
    for (std::shared_ptr<Token> token : tokens)
    {
        std::cout << "<" << token->getType() << ", " << token->getValue() << "> ";
    }

    std::cout << std::endl;
}

void EXPORT debug_output_branch(std::shared_ptr<Branch> branch, int no_tabs)
{
    for (int i = 0; i < no_tabs; i++)
    {
        std::cout << "\t";
    }

    std::cout << branch->getType() << ":" << branch->getValue() << " -> " << branch->getChildren().size() << std::endl;

    for (std::shared_ptr<Branch> child : branch->getChildren())
        debug_output_branch(child, no_tabs + 1);

}
#endif

std::string EXPORT LoadFile(std::string filename)
{
    // Load the file
    std::ifstream ifs;
    std::string source = "";
    ifs.open(filename);
    if (!ifs.is_open())
    {
        throw Exception("Failed to open: " + filename);
    }

    while (ifs.good())
    {
        source += ifs.get();
    }
    ifs.close();

    return source;
}

void EXPORT WriteFile(std::string filename, Stream* stream)
{
    std::ofstream ofs;
    ofs.open(filename, ios::binary);
    if (!ofs.is_open())
    {
        throw Exception("Failed to open: " + filename + " for writing");
    }
    while (!stream->isEmpty())
    {
        ofs << stream->read8();
    }

    ofs.close();
}