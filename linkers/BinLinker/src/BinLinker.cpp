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
#include "common.h"
#include <iostream>

BinLinker::BinLinker(Compiler* compiler) : Linker(compiler)
{
}

BinLinker::~BinLinker()
{
}

void BinLinker::WriteSegment(Stream* executable_stream, std::shared_ptr<VirtualSegment> segment)
{
    std::shared_ptr<Stream> segment_stream = segment->getStream();
    executable_stream->writeStream(segment_stream);
}

int BinLinker::countStreamSizesStopAtSegment(std::shared_ptr<VirtualObjectFormat> obj, std::shared_ptr<VirtualSegment> segment_to_stop)
{
    int size = 0;
    std::string segment_name_to_stop_at = segment_to_stop->getName();
    for (std::shared_ptr<VirtualSegment> segment : obj->getSegments())
    {
        if (segment->getName() == segment_name_to_stop_at)
            break;

        size += segment->getStream()->getSize();
    }
    return size;
}

void BinLinker::resolve(std::shared_ptr<VirtualObjectFormat> final_obj)
{
    // We should resolve the code segment first
    if (final_obj->hasSegment("code"))
    {
        resolve_segment(final_obj, final_obj->getSegment("code"));
    }

    for (std::shared_ptr<VirtualSegment> segment : final_obj->getSegments())
    {
        // We have already resolved the code segment
        if (segment->getName() != "code")
            resolve_segment(final_obj, segment);
    }
}

void BinLinker::resolve_segment(std::shared_ptr<VirtualObjectFormat> final_obj, std::shared_ptr<VirtualSegment> segment)
{
    int segment_abs_pos = countStreamSizesStopAtSegment(final_obj, segment);
    std::shared_ptr<Stream> segment_stream = segment->getStream();
    for (std::shared_ptr<FIXUP> fixup : segment->getFixups())
    {
        int fixup_offset = fixup->getOffset();
        std::shared_ptr<FIXUP_TARGET> target = fixup->getTarget();
        if (target->getType() == FIXUP_TARGET_TYPE_SEGMENT)
        {
            int diff, new_pos;
            std::shared_ptr<FIXUP_TARGET_SEGMENT> fixup_target_segment = std::dynamic_pointer_cast<FIXUP_TARGET_SEGMENT>(target);
            int target_seg_abs_pos = countStreamSizesStopAtSegment(final_obj, fixup_target_segment->getTargetSegment());
            if (fixup->getType() == FIXUP_TYPE_SEGMENT)
            {
                // This is a segment fixup, we are fixing up from the start of a segment
                diff = target_seg_abs_pos;
            }
            else if (fixup->getType() == FIXUP_TYPE_SELF_RELATIVE)
            {
                // This is a self relative fixup, the offset must be relative to the end of the instruction
                int abs_pos_to_offset = segment_abs_pos + fixup_offset + GetFixupLengthAsInteger(fixup->getLength());
                int distance_to_target_segment = (target_seg_abs_pos - abs_pos_to_offset);
                diff = distance_to_target_segment;
            }


            switch (fixup->getLength())
            {
            case FIXUP_8BIT:
                new_pos = diff + segment_stream->peek8(fixup_offset);
                segment_stream->overwrite8(fixup_offset, new_pos);
                break;
            case FIXUP_16BIT:
                new_pos = diff + segment_stream->peek16(fixup_offset);
                segment_stream->overwrite16(fixup_offset, new_pos);
                break;
            case FIXUP_32BIT:
                new_pos = diff + segment_stream->peek32(fixup_offset);
                segment_stream->overwrite32(fixup_offset, new_pos);
                break;
            }
        }
        else if (target->getType() == FIXUP_TARGET_TYPE_EXTERN)
        {
            std::shared_ptr<FIXUP_TARGET_EXTERN> fixup_target_extern = std::dynamic_pointer_cast<FIXUP_TARGET_EXTERN>(target);

            std::shared_ptr<GLOBAL_REF> global_ref = final_obj->getGlobalReferenceByName(fixup_target_extern->getExternalName());
            int global_ref_seg_abs_pos = countStreamSizesStopAtSegment(final_obj, global_ref->getSegment());
            int global_ref_abs_pos = global_ref_seg_abs_pos + global_ref->getOffset();
            int new_pos;
            if (fixup->getType() == FIXUP_TYPE_SEGMENT)
            {
                new_pos = global_ref_abs_pos;
            }
            else if (fixup->getType() == FIXUP_TYPE_SELF_RELATIVE)
            {
                int abs_pos_to_offset = segment_abs_pos + fixup_offset + GetFixupLengthAsInteger(fixup->getLength());
                int distance_to_global_ref = (global_ref_abs_pos - abs_pos_to_offset);
                new_pos = distance_to_global_ref;
            }

            switch (fixup->getLength())
            {
            case FIXUP_8BIT:
                segment_stream->overwrite8(fixup_offset, new_pos);
                break;
            case FIXUP_16BIT:
                segment_stream->overwrite16(fixup_offset, new_pos);
                break;
            case FIXUP_32BIT:
                segment_stream->overwrite32(fixup_offset, new_pos);
                break;
            }

        }
    }
}

void BinLinker::build(Stream* executable_stream, std::shared_ptr<VirtualObjectFormat> final_obj)
{
    // Lets prioritise the code segment
    std::shared_ptr<VirtualSegment> code_segment = final_obj->getSegment("code");
    WriteSegment(executable_stream, code_segment);


    // Now lets write all other segments
    for (std::shared_ptr<VirtualSegment> segment : final_obj->getSegments())
    {
        // We have already processed segment code
        if (segment->getName() == "code")
            continue;

        WriteSegment(executable_stream, segment);
    }
}