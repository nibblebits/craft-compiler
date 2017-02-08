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

void BinLinker::WriteSegment(Stream* executable_stream, std::shared_ptr<VirtualSegment> segment)
{
    Stream* segment_stream = segment->getStream();
    // We need to reset the position of the segment stream
    segment_stream->setPosition(0);
    while(segment_stream->hasInput())
    {
        executable_stream->write8(segment_stream->read8());
    }
}

int BinLinker::countStreamSizesStopAtSegment(std::shared_ptr<VirtualObjectFormat> obj, std::shared_ptr<VirtualSegment> segment_to_stop)
{
    int size = 0;
    for(std::shared_ptr<VirtualSegment> segment : obj->getSegments())
    {
        if (segment == segment_to_stop)
            break;
        
        size += segment->getStream()->getSize();
    }
    return size;
}

void BinLinker::resolve(std::shared_ptr<VirtualObjectFormat> final_obj)
{
    // Lets resolve the offsets
    for(std::shared_ptr<VirtualSegment> segment : final_obj->getSegments())
    {
        Stream* segment_stream = segment->getStream();
        // Lets handle segment fixups
        for (std::shared_ptr<FIXUP> fixup : segment->getFixups())
        {
            if (fixup->getType() == FIXUP_TYPE_SEGMENT)
            {
                std::shared_ptr<SEGMENT_FIXUP> seg_fixup = std::dynamic_pointer_cast<SEGMENT_FIXUP>(fixup);
                int pos_of_rel_seg = countStreamSizesStopAtSegment(final_obj, seg_fixup->getRelatingSegment());
                int old_offset = segment_stream->peek16(seg_fixup->getOffset());
                int new_offset = pos_of_rel_seg + old_offset + segment->getOrigin();
                // Now we need to append the position of the relating segment to this fixup
                segment_stream->overwrite16(seg_fixup->getOffset(), new_offset);
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