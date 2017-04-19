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
 * File:   CraftStaticLibraryFormat.h
 * Author: Daniel McCarthy
 *
 * Created on 13 April 2017, 18:03
 */

#ifndef CRAFTSTATICLIBRARYFORMAT_H
#define CRAFTSTATICLIBRARYFORMAT_H

#include <vector>
#include <memory>
#include "VirtualObjectFormat.h"
#include "def.h"
#include "Stream.h"

#define CSL_DESCRIPTOR "CSL"
#define CSL_VERSION 1

enum {
    FIX_TYPE_SELF_RELATIVE,
    FIX_TYPE_SEGMENT
};

typedef unsigned int _FIX_TYPE;

enum {
    FIX_TARGET_TYPE_SEGMENT,
    FIX_TARGET_TYPE_EXTERN
};

typedef unsigned int _FIX_TARGET_TYPE;

enum {
    _FIXUP_8BIT,
    _FIXUP_16BIT,
    _FIXUP_32BIT,
    _FIXUP_64_BIT,
};

typedef unsigned int _FIXUP_LENGTH;

typedef unsigned char FIXUP_INFO;

struct global_reference {
    std::string name;
    int offset;
};

struct external_reference_fixup {
    std::string name;
    int name_id;
    int offset;
};

struct segment {
    std::string name;
    std::shared_ptr<Stream> data;
    std::vector<struct global_reference> global_refs;
    std::vector<std::shared_ptr<struct FIXUP>> fixups;
};

struct object {
    std::string name;
    std::vector<struct segment> segments;
};

struct header {
    std::string descriptor;
    int version;
};

struct library {
    struct header header;
    std::vector<struct object> objects;
};

class EXPORT CraftStaticLibraryFormat : public VirtualObjectFormat {
public:
    CraftStaticLibraryFormat(Compiler* compiler);
    virtual ~CraftStaticLibraryFormat();

    void addObjectFormat(std::shared_ptr<VirtualObjectFormat> obj_format);

    virtual void read(std::shared_ptr<Stream> input_stream);
    virtual void finalize();

    void finalize_part(struct library* library, std::shared_ptr<VirtualObjectFormat> virtual_obj_format);
protected:
    virtual std::shared_ptr<VirtualSegment> new_segment(std::string segment_name, uint32_t origin);
    std::string ReadString();
    void ReadHeader(struct header* header);
    void ReadObjects(struct library* library);
    void ReadSegments(std::shared_ptr<VirtualObjectFormat> obj_format);
    void ReadFixup(std::shared_ptr<VirtualObjectFormat> obj_format, std::shared_ptr<VirtualSegment> segment);
    void WriteString(Stream* lib_stream, std::string str);
    void WriteHeader(Stream* lib_stream, struct header* header);
    void WriteObjects(Stream* lib_stream, std::vector<struct object> objects);
    void WriteSegments(Stream* lib_stream, std::vector<struct segment> segments);
    void WriteFixup(Stream* lib_stream, std::shared_ptr<struct FIXUP> fixup);
private:
    std::vector<std::shared_ptr<VirtualObjectFormat>> obj_formats;
    std::shared_ptr<Stream> input_stream;
};

#endif /* CRAFTSTATICLIBRARYFORMAT_H */

