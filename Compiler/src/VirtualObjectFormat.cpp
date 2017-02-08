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
#include "Compiler.h"
#include <iostream>
#include <algorithm>
#include <map>

VirtualObjectFormat::VirtualObjectFormat(Compiler* compiler) : CompilerEntity(compiler)
{
}

VirtualObjectFormat::~VirtualObjectFormat()
{
}

std::shared_ptr<VirtualSegment> VirtualObjectFormat::createSegment(std::string segment_name)
{
    uint32_t origin = 0;
    // We need to see if an origin is present, this would have been provided in the arguments upon running the compiler
    std::string argument_name = "org_" + segment_name;
    if(getCompiler()->hasArgument(argument_name))
    {
        // An argument exists so lets set the origin
        origin = std::stoi(getCompiler()->getArgumentValue(argument_name));
    }
    std::shared_ptr<VirtualSegment> segment = std::shared_ptr<VirtualSegment>(new_segment(segment_name, origin));
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

bool VirtualObjectFormat::hasSegment(std::string segment_name)
{
    return getSegment(segment_name) != NULL;
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
    // Check if the reference is already registered.
    if (hasExternalReference(ref_name))
        return;

    this->external_references.push_back(ref_name);
}

std::vector<std::string> VirtualObjectFormat::getExternalReferences()
{
    return this->external_references;
}

bool VirtualObjectFormat::hasExternalReference(std::string ref_name)
{
    return std::find(this->external_references.begin(), this->external_references.end(), ref_name) != this->external_references.end();
}

bool VirtualObjectFormat::hasExternalReferences()
{
    return !this->external_references.empty();
}

Stream* VirtualObjectFormat::getObjectStream()
{
    return &this->object_stream;
}

void VirtualObjectFormat::append(std::shared_ptr<VirtualObjectFormat> obj_format)
{
    // Append any external references
    for (std::string ext_ref : obj_format->getExternalReferences())
    {
        registerExternalReference(ext_ref);
    }

    std::map<std::string, int> old_size_map;
    int old_size;
    std::shared_ptr<VirtualSegment> our_segment;

    // Append the segments, this requires two passes due to relating segments
    for (std::shared_ptr<VirtualSegment> segment : obj_format->getSegments())
    {
        if (hasSegment(segment->getName()))
        {
            our_segment = getSegment(segment->getName());
        }
        else
        {
            // We don't have the segment so we need to create it
            our_segment = createSegment(segment->getName());
        }

        // Log the old size for later we are going to need it to resolve offsets
        old_size = our_segment->getStream()->getSize();
        old_size_map[our_segment->getName()] = old_size;

        // Write the segments stream to our own
        our_segment->getStream()->writeStream(segment->getStream());
    }

    // Pass 2
    for (std::shared_ptr<VirtualSegment> segment : obj_format->getSegments())
    {
        our_segment = getSegment(segment->getName());
        int old_size = old_size_map[segment->getName()];
        // Relocatable offsets of the new stream are now wrong so we need to fix them and add them to our segment
        for (std::shared_ptr<FIXUP> fixup : segment->getFixups())
        {
            std::shared_ptr<FIXUP_STANDARD> fixup_standard = std::dynamic_pointer_cast<FIXUP_STANDARD>(fixup);
            fixup_standard->appendOffset(old_size);
            if (fixup->getType() == FIXUP_TYPE_SEGMENT)
            {
                std::shared_ptr<SEGMENT_FIXUP> seg_fixup = std::dynamic_pointer_cast<SEGMENT_FIXUP>(fixup);
                // We want a new fix up based on these changes 
                std::shared_ptr<VirtualSegment> relating_seg = getSegment(seg_fixup->getRelatingSegment()->getName());
                int new_data_offset = old_size_map[relating_seg->getName()] + our_segment->getStream()->peek16(seg_fixup->getOffset());
                our_segment->getStream()->overwrite16(seg_fixup->getOffset(), new_data_offset);
                our_segment->register_fixup(relating_seg, seg_fixup->getOffset(), seg_fixup->getFixupLength());
            }
            else if(fixup->getType() == FIXUP_TYPE_EXTERN)
            {
                std::shared_ptr<EXTERN_FIXUP> extern_fixup = std::dynamic_pointer_cast<EXTERN_FIXUP>(fixup);
                our_segment->register_fixup_extern(extern_fixup->getExternalName(), extern_fixup->getOffset(), extern_fixup->getFixupLength());
            }
        }

        // We should also add the global references
        for (std::shared_ptr<GLOBAL_REF> global_ref : segment->getGlobalReferences())
        {
            our_segment->register_global_reference(global_ref->getName(), global_ref->getOffset());
        }
    }

}