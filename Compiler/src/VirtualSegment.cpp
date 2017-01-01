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
 * File:   VirtualSegment.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 15:48
 * 
 * Description: 
 */

#include "VirtualSegment.h"

VirtualSegment::VirtualSegment(std::string segment_name)
{
    this->segment_name = segment_name;
}

VirtualSegment::~VirtualSegment()
{
}

std::string VirtualSegment::getName()
{
    return this->segment_name;
}

Stream* VirtualSegment::getStream()
{
    return &this->stream;
}