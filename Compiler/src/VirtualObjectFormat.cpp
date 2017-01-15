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
 * File:   VirtualObjectFormat.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 15:41
 * 
 * Description: 
 */

#include "VirtualObjectFormat.h"
#include "VirtualSegment.h"
#include <iostream>
VirtualObjectFormat::VirtualObjectFormat(Compiler* compiler) : CompilerEntity(compiler)
{

}

VirtualObjectFormat::~VirtualObjectFormat()
{
}

std::shared_ptr<VirtualSegment> VirtualObjectFormat::createSegment(std::string segment_name)
{
    std::shared_ptr<VirtualSegment> segment = std::shared_ptr<VirtualSegment>(new_segment(segment_name));
    std::cout << segments.size() << std::endl;
    this->segments.push_back(segment);
    return segment;
}

std::shared_ptr<VirtualSegment> VirtualObjectFormat::getSegment(std::string segment_name)
{
    for (std::shared_ptr<VirtualSegment> segment : this->segments)
    {
        if (segment->getName() == segment_name)
            return segment;
    }
    
    return NULL;
}

std::vector<std::shared_ptr<VirtualSegment>> VirtualObjectFormat::getSegments()
{
    return this->segments;
}
        

Stream* VirtualObjectFormat::getObjectStream()
{
    return &this->object_stream;
}