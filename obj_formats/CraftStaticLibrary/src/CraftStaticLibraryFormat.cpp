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
 * File:   CraftStaticLibraryFormat.cpp
 * Author: Daniel McCarthy
 *
 * Created on 13 April 2017, 18:03
 * 
 * Description: 
 */

#include <cstring>
#include "CraftStaticLibraryFormat.h"
#include "BlankVirtualObjectFormat.h"
#include "Compiler.h"

CraftStaticLibraryFormat::CraftStaticLibraryFormat(Compiler* compiler) : VirtualObjectFormat(compiler)
{
    this->input_stream = NULL;
}

CraftStaticLibraryFormat::~CraftStaticLibraryFormat()
{
}

void CraftStaticLibraryFormat::addObjectFormat(std::shared_ptr<VirtualObjectFormat> obj_format)
{
    this->obj_formats.push_back(obj_format);
}

std::string CraftStaticLibraryFormat::ReadString()
{
    int str_size = input_stream->read8();
    char str_buf[str_size + 1];
    input_stream->read(str_buf, str_size);
    // Add null terminator
    str_buf[str_size] = 0;

    return std::string(str_buf);
}

void CraftStaticLibraryFormat::ReadHeader(struct header* header)
{
    input_stream->read(&header->descriptor, 3);
    if (header->descriptor != "CSL")
    {
        throw Exception("The file \"" + getFileName() + "\" is not a craft static library", "void CraftStaticLibraryFormat::ReadHeader(struct header* header)");
    }

    header->version = input_stream->read16();
    if (header->version > CSL_VERSION)
    {
        throw Exception("This craft static library is of a version that this library cannot handle. Please upgrade your compiler", "void CraftStaticLibraryFormat::ReadHeader(struct header* header)");
    }

    // Reserved byte
    input_stream->read8();
}

void CraftStaticLibraryFormat::ReadObjects(struct library* library)
{
    int total_objs = input_stream->read8();
    for (int i = 0; i < total_objs; i++)
    {
        // Get the object name and create a new object
        std::string obj_name = ReadString();
        std::shared_ptr<VirtualObjectFormat> obj_format = std::shared_ptr<VirtualObjectFormat>(new BlankVirtualObjectFormat(getCompiler()));
        obj_format->setFileName(obj_name);
        ReadSegments(obj_format);
        getCompiler()->getLinker()->addObjectFile(obj_format);
    }
}

void CraftStaticLibraryFormat::ReadSegments(std::shared_ptr<VirtualObjectFormat> obj_format)
{
    int total_segments = input_stream->read8();
    for (int i = 0; i < total_segments; i++)
    {
        std::string seg_name = ReadString();
        std::shared_ptr<VirtualSegment> segment = obj_format->createSegment(seg_name);
        int seg_data_size = input_stream->read32();
        for (int i = 0; i < seg_data_size; i++)
        {
            std::shared_ptr<Stream> seg_stream = segment->getStream();
            seg_stream->write8(input_stream->read8());
        }

        // Read global references
        int total_refs = input_stream->read8();
        for (int i = 0; i < total_refs; i++)
        {
            std::string global_name = ReadString();
            int offset = input_stream->read32();
            segment->register_global_reference(global_name, offset);
        }

        // Load fixups
        int total_fixups = input_stream->read32();
        for (int i = 0; i < total_fixups; i++)
        {
            ReadFixup(obj_format, segment);
        }
    }
}

void CraftStaticLibraryFormat::ReadFixup(std::shared_ptr<VirtualObjectFormat> obj_format, std::shared_ptr<VirtualSegment> segment)
{
    FIXUP_INFO fixup_info = input_stream->read8();
    _FIXUP_LENGTH fixup_len = fixup_info >> 6;
    _FIX_TYPE fixup_type = (fixup_info >> 3) & 0x07;
    _FIX_TARGET_TYPE fix_target_type = fixup_info & 0x03;

    FIXUP_LENGTH real_fixup_length;
    FIXUP_TYPE real_fixup_type;

    if (fixup_len == _FIXUP_8BIT)
    {
        real_fixup_length = FIXUP_8BIT;
    }
    else if (fixup_len == _FIXUP_16BIT)
    {
        real_fixup_length = FIXUP_16BIT;
    }
    else if (fixup_len == _FIXUP_32BIT)
    {
        real_fixup_length == FIXUP_32BIT;
    }
    else if (fixup_len == _FIXUP_64_BIT)
    {
        real_fixup_length = FIXUP_64_BIT;
    }
    else
    {
        throw Exception("Invalid fixup length", "void CraftStaticLibraryFormat::ReadFixup(std::shared_ptr<VirtualSegment> segment)");
    }

    if (fixup_type == FIX_TYPE_SELF_RELATIVE)
    {
        real_fixup_type = FIXUP_TYPE_SELF_RELATIVE;
    }
    else if (fixup_type == FIX_TYPE_SEGMENT)
    {
        real_fixup_type = FIXUP_TYPE_SEGMENT;
    }
    else
    {
        throw Exception("Invalid fixup type", "void CraftStaticLibraryFormat::ReadFixup(std::shared_ptr<VirtualSegment> segment)");
    }

    if (fix_target_type == FIX_TARGET_TYPE_SEGMENT)
    {
        std::string relating_seg_name = ReadString();
        int offset = input_stream->read32();
        std::shared_ptr<VirtualSegment> relating_seg = obj_format->getSegment(relating_seg_name);
        segment->register_fixup_target_segment(real_fixup_type, relating_seg, offset, real_fixup_length);
    }
    else if (fix_target_type == FIX_TARGET_TYPE_EXTERN)
    {
        std::string external_name = ReadString();
        int offset = input_stream->read32();
        segment->register_fixup_target_extern(real_fixup_type, external_name, offset, real_fixup_length);
    }
    else
    {
        throw Exception("Unsupported fix target while reading from object file", "void CraftStaticLibraryFormat::ReadFixup(std::shared_ptr<VirtualSegment> segment)");
    }

}

void CraftStaticLibraryFormat::read(std::shared_ptr<Stream> input_stream)
{
    this->input_stream = input_stream;
    struct library library;
    ReadHeader(&library.header);
    ReadObjects(&library);
}

void CraftStaticLibraryFormat::WriteString(Stream* lib_stream, std::string str)
{
    if (str.size() > 255)
    {
        throw Exception("The string \"" + str + "\" exceeds the maximum of 255 bytes in length", "void CraftStaticLibraryFormat::WriteString(Stream* lib_stream, std::string str)");
    }
    lib_stream->write8(str.size());
    lib_stream->writeStr(str, false);
}

void CraftStaticLibraryFormat::WriteHeader(Stream* lib_stream, struct header* header)
{
    if (header->descriptor.size() > 3)
    {
        throw Exception("Descriptor may may only be 3 bytes", "void CraftStaticLibraryFormat::WriteHeader(Stream* lib_stream, struct header* header)");
    }
    lib_stream->writeStr(header->descriptor, false);
    lib_stream->write16(header->version);
    // Reserved byte
    lib_stream->write8(0);
}

void CraftStaticLibraryFormat::WriteObjects(Stream* lib_stream, std::vector<struct object> objects)
{
    if (objects.size() > 255)
    {
        throw Exception("Only a maximum of 255 objects are supported", "void CraftStaticLibraryFormat::WriteObjects(Stream* lib_stream, std::vector<struct object> objects)");
    }
    lib_stream->write8(objects.size());
    for (struct object obj : objects)
    {
        WriteString(lib_stream, obj.name);
        WriteSegments(lib_stream, obj.segments);
    }
}

void CraftStaticLibraryFormat::WriteSegments(Stream* lib_stream, std::vector<struct segment> segments)
{
    if (segments.size() > 255)
    {
        throw Exception("Only a maximum of 255 segments are supported", "void CraftStaticLibraryFormat::WriteSegments(Stream* lib_stream, std::vector<struct segment> segments)");
    }
    lib_stream->write8(segments.size());
    for (struct segment segment : segments)
    {
        WriteString(lib_stream, segment.name);
        lib_stream->write32(segment.data->getSize());
        lib_stream->writeStream(segment.data);

        // Write global references
        lib_stream->write8(segment.global_refs.size());
        for (struct global_reference global_ref : segment.global_refs)
        {
            WriteString(lib_stream, global_ref.name);
            lib_stream->write32(global_ref.offset);
        }

        // Total fixups
        lib_stream->write32(segment.fixups.size());
        for (std::shared_ptr<struct FIXUP> fixup : segment.fixups)
        {
            WriteFixup(lib_stream, fixup);
        }
    }
}

void CraftStaticLibraryFormat::WriteFixup(Stream* lib_stream, std::shared_ptr<struct FIXUP> fixup)
{
    FIXUP_INFO fixup_info = 0;

    // The length is stored in the fixup info from bit 7 to bit 5
    switch (fixup->getLength())
    {
    case FIXUP_8BIT:
        fixup_info |= (_FIXUP_8BIT << 6);
        break;
    case FIXUP_16BIT:
        fixup_info |= (_FIXUP_16BIT << 6);
        break;
    case FIXUP_32BIT:
        fixup_info |= (_FIXUP_32BIT << 6);
        break;
    default:
        throw Exception("Length is not supported", "void CraftStaticLibraryFormat::WriteFixup(Stream* lib_stream, std::shared_ptr<struct FIXUP> fixup)");
    }


    // Fixup types are stored in the fixup info from bit 5 to bit 2 of the fixup info
    switch (fixup->getType())
    {
    case FIXUP_TYPE_SELF_RELATIVE:
        fixup_info |= (FIX_TYPE_SELF_RELATIVE << 3);
        break;
    case FIXUP_TYPE_SEGMENT:
        fixup_info |= (FIX_TYPE_SEGMENT << 3);
        break;
    default:
        throw Exception("Unsupported fixup type", "void CraftStaticLibraryFormat::WriteFixup(Stream* lib_stream, std::shared_ptr<struct FIXUP> fixup)");
    }

    // Fixup targets are stored from bit 2 to bit 0
    switch (fixup->getTarget()->getType())
    {
    case FIXUP_TARGET_TYPE_SEGMENT:
    {
        std::shared_ptr<struct FIXUP_TARGET_SEGMENT> targt_seg = std::dynamic_pointer_cast<struct FIXUP_TARGET_SEGMENT>(fixup->getTarget());
        fixup_info |= FIX_TARGET_TYPE_SEGMENT;
        // Write the fixup info byte
        lib_stream->write8(fixup_info);
        WriteString(lib_stream, targt_seg->getTargetSegment()->getName());
    }
        break;
    case FIXUP_TARGET_TYPE_EXTERN:
    {
        std::shared_ptr<struct FIXUP_TARGET_EXTERN> targt_extern = std::dynamic_pointer_cast<struct FIXUP_TARGET_EXTERN>(fixup->getTarget());
        fixup_info |= FIX_TARGET_TYPE_EXTERN;
        // Write the fixup info byte
        lib_stream->write8(fixup_info);
        WriteString(lib_stream, targt_extern->getExternalName());
    }
        break;
    default:
        throw Exception("Unsupported fixup target type", "void CraftStaticLibraryFormat::WriteFixup(Stream* lib_stream, std::shared_ptr<struct FIXUP> fixup)");
    }

    lib_stream->write32(fixup->getOffset());
}

void CraftStaticLibraryFormat::finalize_part(struct library* library, std::shared_ptr<VirtualObjectFormat> virtual_obj_format)
{
    struct object obj;
    obj.name = virtual_obj_format->getFileName();
    for (std::shared_ptr<VirtualSegment> segment : virtual_obj_format->getSegments())
    {
        struct segment seg;
        seg.name = segment->getName();
        seg.data = segment->getStream();

        for (std::shared_ptr<GLOBAL_REF> _global_ref : segment->getGlobalReferences())
        {
            struct global_reference global_ref;
            global_ref.name = _global_ref->getName();
            global_ref.offset = _global_ref->getOffset();
            seg.global_refs.push_back(global_ref);
        }

        for (std::shared_ptr<struct FIXUP> fixup : segment->getFixups())
        {
            seg.fixups.push_back(fixup);
        }
        obj.segments.push_back(seg);

    }

    library->objects.push_back(obj);
}

void CraftStaticLibraryFormat::finalize()
{
    struct library library;
    struct header header;
    header.descriptor = std::string(CSL_DESCRIPTOR);
    header.version = CSL_VERSION;
    library.header = header;

    // Finalize ourself
    finalize_part(&library, std::dynamic_pointer_cast<VirtualObjectFormat>(getptr()));

    // Finalize all objects attached to us
    for (std::shared_ptr<VirtualObjectFormat> virtual_obj_format : this->obj_formats)
    {
        finalize_part(&library, virtual_obj_format);
    }

    // We are ready to generate the file
    Stream* lib_stream = getObjectStream();

    // Lets write the header
    WriteHeader(lib_stream, &header);

    // Lets write the object information
    WriteObjects(lib_stream, library.objects);
}

std::shared_ptr<VirtualSegment> CraftStaticLibraryFormat::new_segment(std::string segment_name, uint32_t origin)
{
    return std::shared_ptr<VirtualSegment>(new VirtualSegment(segment_name, origin));
}