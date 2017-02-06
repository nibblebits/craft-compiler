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

void VirtualObjectFormat::registerGlobalReference(std::shared_ptr<VirtualSegment> segment, std::string ref_name, int offset)
{
    if (segment == NULL)
    {
        throw Exception("void VirtualObjectFormat::registerGlobalReference(std::shared_ptr<VirtualSegment> segment, std::string ref_name, int offset): expecting a segment but NULL was provided");
    }

    segment->register_global_reference(ref_name, offset);
}

std::vector<std::shared_ptr<GLOBAL_REF>> VirtualObjectFormat::getGlobalReferences()
{
    std::vector<std::shared_ptr < GLOBAL_REF>> references;
    for (std::shared_ptr<VirtualSegment> segment : getSegments())
    {
        std::vector<std::shared_ptr < GLOBAL_REF>> seg_refs = segment->getGlobalReferences();
        references.insert(references.end(), seg_refs.begin(), seg_refs.end());
    }

    return references;
}

std::vector<std::shared_ptr<GLOBAL_REF>> VirtualObjectFormat::getGlobalReferencesForSegment(std::string segment_name)
{
    return getSegment(segment_name)->getGlobalReferences();
}

void VirtualObjectFormat::registerExternalReference(std::string ref_name)
{
    this->external_references.push_back(ref_name);
}

std::vector<std::string> VirtualObjectFormat::getExternalReferences()
{
    return this->external_references;
}

bool VirtualObjectFormat::hasExternalReferences()
{
    return !this->external_references.empty();
}

Stream* VirtualObjectFormat::getObjectStream()
{
    return &this->object_stream;
}