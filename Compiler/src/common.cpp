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
 * File:   common.h
 * Author: Daniel McCarthy
 *
 * Created on 14 September 2016, 01:26
 * 
 * Description: Common functions are held here
 */

#include "common.h"
#include "ScopeBranch.h"

using namespace std;

#ifdef DEBUG_MODE

#include "VirtualObjectFormat.h"

void EXPORT debug_output_tokens(std::vector<std::shared_ptr<Token>> tokens)
{
    std::cout << "DEBUG TOKEN OUTPUT" << std::endl;
    for (std::shared_ptr<Token> token : tokens)
    {
        std::cout << "<" << token->getType() << ", " << token->getValue() << "> ";
    }

    std::cout << std::endl;
}

void EXPORT debug_output_branch(std::shared_ptr<Branch> branch, int no_tabs)
{
    for (int i = 0; i < no_tabs; i++)
    {
        std::cout << "\t";
    }

    std::shared_ptr<Branch> local_scope = branch->getLocalScope();
    if (local_scope != NULL)
    {
        std::shared_ptr<Branch> local_scope_parent = local_scope->getParent();
        std::cout << branch->getType() << ":" << branch->getValue() << " -> " << branch->getChildren().size() << " -> "
                << branch->getLocalScope()->getType() << " sp: " << ((local_scope_parent != NULL) ? local_scope_parent->getType() : " NO SCOPE PARENT") << std::endl;
    }
    else
    {
        std::cout << branch->getType() << ":" << branch->getValue() << " -> " << branch->getChildren().size() << " -> " << "NO SCOPE" << std::endl;
    }


    for (std::shared_ptr<Branch> child : branch->getChildren())
        debug_output_branch(child, no_tabs + 1);

}

void debug_virtual_object_format_segment(std::shared_ptr<VirtualSegment> segment)
{
    std::shared_ptr<Stream> segment_stream = segment->getStream();
    std::cout << "\tSEGMENT: " << segment->getName() << ", segment address: " << segment.get() << std::endl;

    if (segment->hasFixups())
    {
        std::cout << "\t" << "Displaying FIXUPS" << std::endl;
        for (std::shared_ptr<FIXUP> fixup : segment->getFixups())
        {
            int fixup_offset = fixup->getOffset();
            std::cout << "\t\t";
            std::shared_ptr<FIXUP_TARGET> fixup_target = fixup->getTarget();
            std::cout << "Fixup type: " << fixup->getTypeAsString() << ", targeting: " << fixup_target->getTypeAsString();
            if (fixup_target->getType() == FIXUP_TARGET_TYPE_SEGMENT)
            {
                int offset_val;
                switch (fixup->getLength())
                {
                case FIXUP_8BIT:
                    offset_val = segment_stream->peek8(fixup_offset);
                    break;
                case FIXUP_16BIT:
                    offset_val = segment_stream->peek16(fixup_offset);
                    break;
                case FIXUP_32BIT:
                    offset_val = segment_stream->peek32(fixup_offset);
                    break;
                }
                std::shared_ptr<FIXUP_TARGET_SEGMENT> fixup_target_segment = std::dynamic_pointer_cast<FIXUP_TARGET_SEGMENT>(fixup_target);
                std::cout << " target segment: " << fixup_target_segment->getTargetSegment()->getName() << ", offset value: " << std::to_string(offset_val);
            }
            else if (fixup_target->getType() == FIXUP_TARGET_TYPE_EXTERN)
            {
                std::shared_ptr<FIXUP_TARGET_EXTERN> fixup_target_extern = std::dynamic_pointer_cast<FIXUP_TARGET_EXTERN>(fixup_target);
                std::cout << " target extern: " << fixup_target_extern->getExternalName() << ",";
            }

            std::cout << " offset: " << fixup_offset << " fixup length: " << std::to_string(GetFixupLengthAsInteger(fixup->getLength())) << std::endl;
        }
    }
    else
    {
        std::cout << "\t" << "No FIXUPS to display" << std::endl;
    }

    if (segment->hasGlobalReferences())
    {
        std::cout << "\t" << "Displaying global exported references" << std::endl;
        for (std::shared_ptr<GLOBAL_REF> global_ref : segment->getGlobalReferences())
        {
            std::cout << "\t\t";
            std::cout << "GLOBAL REF: " << global_ref->getName() << ", segment: " << global_ref->getSegment()->getName() <<
                    " offset: " << global_ref->getOffset() << std::endl;
        }
    }
    else
    {
        std::cout << "\t" << "No global exported references to display" << std::endl;
    }

}

void debug_virtual_object_format(std::shared_ptr<VirtualObjectFormat> virtual_object_format)
{
    std::cout << "Total segments: " << virtual_object_format->getSegments().size() << std::endl;
    for (std::shared_ptr<VirtualSegment> segment : virtual_object_format->getSegments())
    {
        debug_virtual_object_format_segment(segment);
    }
}

#endif

std::ifstream::pos_type EXPORT GetFileSize(std::string filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    std::ifstream::pos_type pos = in.tellg();
    in.close();
    return pos;
}

std::shared_ptr<Stream> EXPORT LoadFile(std::string filename)
{
    // Load the file
    std::ifstream ifs;
    std::shared_ptr<Stream> stream = std::shared_ptr<Stream>(new Stream());

    ifs.open(filename, ios::in | ios::binary);
    if (!ifs.is_open())
    {
        throw Exception("Failed to open: " + filename);
    }

    if (ifs.good())
    {
        // Get the file length
        ifs.seekg(0, ifs.end);
        int length = ifs.tellg();
        ifs.seekg(0, ifs.beg);
        char* buf = new char[length];
        ifs.read(buf, length);
        if (!ifs.good())
        {
            throw Exception("Managed to open file: " + filename + " but failed with reading.");
        }
        
        for (int i = 0; i < length; i++)
        {
            stream->write8(buf[i]);
        }
        
        delete[] buf;
    }

    ifs.close();

    return stream;
}

void EXPORT WriteFile(std::string filename, Stream* stream)
{
    // Reset the position
    stream->setPosition(0);
    std::ofstream ofs;
    ofs.open(filename, ios::binary);
    if (!ofs.is_open())
    {
        throw Exception("Failed to open: " + filename + " for writing");
    }
    while (stream->hasInput())
    {
        ofs << stream->read8();
    }

    ofs.close();
}

int EXPORT GetFixupLengthAsInteger(FIXUP_LENGTH fixup_len)
{
    int len = -1;
    switch (fixup_len)
    {
    case FIXUP_8BIT:
        len = 1;
        break;
    case FIXUP_16BIT:
        len = 2;
        break;
    case FIXUP_32BIT:
        len = 4;
        break;
    }

    if (len == -1)
    {
        throw Exception("Invalid fixup length provided", "int GetFixupLengthAsInteger(FIXUP_LENGTH fixup_len)");
    }
    return len;
}

std::string EXPORT GetCompilerName()
{
    return COMPILER_FULLNAME;
}