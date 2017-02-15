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
    if (hasSegment(segment_name))
    {
        throw Exception("The segment: " + segment_name + " already exists.", "std::shared_ptr<VirtualSegment> VirtualObjectFormat::createSegment(std::string segment_name)");
    }
    uint32_t origin = 0;
    // We need to see if an origin is present, this would have been provided in the arguments upon running the compiler
    std::string argument_name = "org_" + segment_name;
    if (getCompiler()->hasArgument(argument_name))
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

bool VirtualObjectFormat::hasGlobalReference(std::string ref_name)
{
    for (std::shared_ptr<VirtualSegment> segment : getSegments())
    {
        if (segment->hasGlobalReference(ref_name))
            return true;
    }

    return false;
}

std::shared_ptr<GLOBAL_REF> VirtualObjectFormat::getGlobalReferenceByName(std::string ref_name)
{
    for (std::shared_ptr<VirtualSegment> segment : getSegments())
    {
        if (segment->hasGlobalReference(ref_name))
            return segment->getGlobalReferenceByName(ref_name);
    }

    return NULL;
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

    // Merge all the same segments together
    for (std::shared_ptr<VirtualSegment> segment : obj_format->getSegments())
    {
        std::shared_ptr<VirtualSegment> main_segment;
        if (hasSegment(segment->getName()))
        {
            main_segment = getSegment(segment->getName());
        }
        else
        {
            main_segment = createSegment(segment->getName());
        }

        // Lets join their streams to the main segment stream
        std::shared_ptr<Stream> main_segment_stream = main_segment->getStream();
        std::shared_ptr<Stream> segment_stream = segment->getStream();
        main_segment_stream->joinStream(segment_stream);
    }

    // Apply fixups where appropriate
    for (std::shared_ptr<VirtualSegment> segment : obj_format->getSegments())
    {
        std::shared_ptr<VirtualSegment> main_segment = getSegment(segment->getName());
        std::shared_ptr<Stream> main_segment_stream = main_segment->getStream();
        std::shared_ptr<Stream> segment_stream = segment->getStream();

        int abs_segment_pos = main_segment_stream->getJoinedStreamPosition(segment_stream);

        // Handle fixups
        for (std::shared_ptr<FIXUP> fixup : segment->getFixups())
        {
            std::shared_ptr<FIXUP_TARGET> target = fixup->getTarget();
            if (target->getType() == FIXUP_TARGET_TYPE_SEGMENT)
            {
                std::shared_ptr<FIXUP_TARGET_SEGMENT> fixup_target_segment = std::dynamic_pointer_cast<FIXUP_TARGET_SEGMENT>(target);
                // We need the absolute position of the segment on the main stream
                std::shared_ptr<VirtualSegment> target_segment = fixup_target_segment->getTargetSegment();
                std::shared_ptr<VirtualSegment> main_target_segment = getSegment(target_segment->getName());
                int fixup_offset = fixup->getOffset();
                int abs_pos_to_stream = main_target_segment->getStream()->getJoinedStreamPosition(target_segment->getStream());
                int new_pos;
                // Now lets fix the offset
                switch (fixup->getLength())
                {
                case FIXUP_8BIT:
                    new_pos = abs_pos_to_stream + segment_stream->peek8(fixup_offset);
                    segment_stream->overwrite8(fixup_offset, new_pos);
                    break;
                case FIXUP_16BIT:
                    new_pos = abs_pos_to_stream + segment_stream->peek16(fixup_offset);
                    segment_stream->overwrite16(fixup_offset, new_pos);
                    break;
                case FIXUP_32BIT:
                    new_pos = abs_pos_to_stream + segment_stream->peek32(fixup_offset);
                    segment_stream->overwrite32(fixup_offset, new_pos);
                    break;
                }
            }
            // We need to add the fixup to the main segment
            int new_offset_pos = abs_segment_pos + fixup->getOffset();
            main_segment->register_fixup(fixup->getTarget(), fixup->getType(), new_offset_pos, fixup->getLength());
        }

        // Handle globals
        for (std::shared_ptr<GLOBAL_REF> global : segment->getGlobalReferences())
        {
            int new_offset_pos = abs_segment_pos + global->getOffset();
            main_segment->register_global_reference(global->getName(), new_offset_pos);
        }
    }
}
