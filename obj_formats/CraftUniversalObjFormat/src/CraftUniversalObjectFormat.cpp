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
 * File:   CraftUniversalObjectFormat.cpp
 * Author: Daniel McCarthy
 *
 * Created on 21 December 2016, 13:48
 * 
 * Description: 
 */

#include "CraftUniversalObjectFormat.h"
#include "UOFSegment.h"

CraftUniversalObjectFormat::CraftUniversalObjectFormat(Compiler* compiler) : VirtualObjectFormat(compiler)
{
}

CraftUniversalObjectFormat::~CraftUniversalObjectFormat()
{
}

void CraftUniversalObjectFormat::finalize()
{
    throw Exception("void CraftUniversalObjectFormat::finalize(): The craft universal object format is currently unimplemented");
}

std::shared_ptr<VirtualSegment> CraftUniversalObjectFormat::new_segment(std::string segment_name)
{
    return std::shared_ptr<UOFSegment>(new UOFSegment(segment_name));
}