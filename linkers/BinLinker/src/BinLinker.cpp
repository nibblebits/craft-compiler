/*
    Craft compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   BinLinker.cpp
 * Author: Daniel McCarthy
 *
 * Created on 06 February 2017, 19:09
 * 
 * Description: 
 */

#include "BinLinker.h"

BinLinker::BinLinker(Compiler* compiler) : Linker(compiler)
{
}

BinLinker::~BinLinker()
{
}

void BinLinker::resolve(std::shared_ptr<VirtualObjectFormat> final_obj)
{

}

void BinLinker::build(Stream* executable_stream, std::shared_ptr<VirtualObjectFormat> final_obj)
{

}