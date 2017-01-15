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
 * File:   OMFObjectFormat.cpp
 * Author: Daniel McCarthy
 *
 * Created on 11 January 2017, 22:10
 * 
 * Description: 
 */

#include "OMFObjectFormat.h"
#include "Compiler.h"

OMFObjectFormat::OMFObjectFormat(Compiler* compiler) : VirtualObjectFormat(compiler)
{
}

OMFObjectFormat::~OMFObjectFormat()
{
}

std::shared_ptr<VirtualSegment> OMFObjectFormat::new_segment(std::string segment_name)
{
    // We are not doing anything special so lets just return the standard virtual segment
    return std::shared_ptr<VirtualSegment>(new VirtualSegment(segment_name));
}

void OMFObjectFormat::finalize()
{
    // Lets create a Magic OMF handle using the MagicOMF library that was written for this library
    struct MagicOMFHandle* handle = MagicOMFCreateHandle();
    // Create the THEADR, this should be the input filename
    MagicOMFAddTHEADR(handle, getCompiler()->getArgumentValue("input").c_str());
    // Lets write some information to explain to a viewer who made the object file. In our case Craft Compiler.
    MagicOMFAddCOMENT(handle,
                      COMENT_NO_PURGE,
                      COMENT_CLASS_TRANSLATOR,
                      COMPILER_FULLNAME
                      );



    // We need to create LNAMES for the segments
    struct RECORD* record = MagicOMFNewLNAMESRecord(handle);
    for (std::shared_ptr<VirtualSegment> segment : getSegments())
    {
        MagicOMFAddLNAME(record, segment->getName().c_str());
    }

    MagicOMFFinishLNAMES(record);

    // We now need to create SEGDEF_16 records for the segments
    for (std::shared_ptr<VirtualSegment> segment : getSegments())
    {
        struct Attributes attributes;
        attributes.A = SEG_ATTR_ALIGNMENT_RELOC_BYTE_ALIGNED;
        attributes.C = SEG_ATTR_COMBINATION_PUBLIC_2;
        attributes.B = 0;
        attributes.P = SEG_ATTR_P_USE16;
        MagicOMFAddSEGDEF16(handle, segment->getName().c_str(), attributes, segment->getStream()->getSize());
    }
    
    // Now we need to create the LEDATA records
    for (std::shared_ptr<VirtualSegment> segment : getSegments())
    {
        Stream* stream = segment->getStream();
        MagicOMFAddLEDATA16(handle, segment->getName().c_str(), 0, stream->getSize(), stream->getBuf());
    }
    
    // Let us build the buffer
    MagicOMFGenerateBuffer(handle);

    // We now have the OMF object in the handles buffer
    for (int i = 0; i < handle->buf_size; i++)
    {
        getObjectStream()->write8(handle->buf[i]);
    }

}