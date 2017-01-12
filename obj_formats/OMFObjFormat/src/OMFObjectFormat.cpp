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
    // Just writes information to explain to a viewer who made the object file. In our case Craft Compiler.
    //write_about_comment();
    
    // We need to create LNAMES for the segment
    for (std::shared_ptr<VirtualSegment> segment : getSegments())
    {
        
    }
    
    // Let us build the buffer
    MagicOMFGenerateBuffer(handle);
    
    // We now have the OMF object in the handles buffer
    for (int i = 0; i < handle->buf_size; i++)
    {
        getObjectStream()->write8(handle->buf[i]);
    }
    
}