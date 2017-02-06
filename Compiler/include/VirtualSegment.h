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
 * File:   VirtualSegment.h
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 15:48
 */

#ifndef VIRTUALSEGMENT_H
#define VIRTUALSEGMENT_H

#include <vector>
#include <string>
#include <memory>
#include "Stream.h"
#include "def.h"

class VirtualSegment;

enum
{
    FIXUP_TYPE_SEGMENT,
    FIXUP_TYPE_EXTERN
};

typedef int FIXUP_TYPE;

class EXPORT FIXUP
{
public:
    FIXUP();
    virtual ~FIXUP();
    virtual FIXUP_TYPE getType() = 0;
private:

};

class EXPORT FIXUP_STANDARD : public FIXUP
{
public:
    FIXUP_STANDARD(int offset, FIXUP_LENGTH length);
    virtual ~FIXUP_STANDARD();
    int getOffset();
    FIXUP_LENGTH getFixupLength();
    virtual FIXUP_TYPE getType() = 0;
private:
    int offset;
    FIXUP_LENGTH length;
};

class EXPORT SEGMENT_FIXUP : public FIXUP_STANDARD
{
public:
    SEGMENT_FIXUP(std::shared_ptr<VirtualSegment> relating_segment, int offset, FIXUP_LENGTH length);
    virtual ~SEGMENT_FIXUP();
    std::shared_ptr<VirtualSegment> getRelatingSegment();
    virtual FIXUP_TYPE getType();
private:
    // The segment where the data for a fixup would be located.
    std::shared_ptr<VirtualSegment> relating_segment;
};

class EXPORT EXTERN_FIXUP : public FIXUP_STANDARD
{
public:
    EXTERN_FIXUP(std::string extern_name, int offset, FIXUP_LENGTH length);
    virtual ~EXTERN_FIXUP();
    std::string getExternalName();
    virtual FIXUP_TYPE getType();
private:
    std::string extern_name;
};

class EXPORT GLOBAL_REF
{
public:
    GLOBAL_REF(std::shared_ptr<VirtualSegment> segment, std::string ref_name, int offset);
    virtual ~GLOBAL_REF();

    std::shared_ptr<VirtualSegment> getSegment();
    std::string getName();
    int getOffset();
private:
    std::shared_ptr<VirtualSegment> segment;
    std::string ref_name;
    int offset;
};

class EXPORT VirtualSegment : public std::enable_shared_from_this<VirtualSegment>
{
public:
    VirtualSegment(std::string segment_name);
    virtual ~VirtualSegment();
    std::string getName();
    Stream* getStream();

    void register_fixup(std::shared_ptr<VirtualSegment> relating_segment, int offset, FIXUP_LENGTH length);
    void register_fixup_extern(std::string extern_name, int offset, FIXUP_LENGTH length);
    void register_global_reference(std::string ref_name, int offset);
    std::vector<std::shared_ptr<FIXUP>> getFixups();
    bool hasFixups();

    std::vector<std::shared_ptr<GLOBAL_REF>> getGlobalReferences();
    bool hasGlobalReferences();

private:
    std::vector<std::shared_ptr<FIXUP>> fixups;
    std::vector<std::shared_ptr<GLOBAL_REF>> global_references;
    std::string segment_name;
    Stream stream;
};

#endif /* VIRTUALSEGMENT_H */

