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
    FIXUP_TYPE_SELF_RELATIVE,
    FIXUP_TYPE_SEGMENT
};

typedef int FIXUP_TYPE;

enum
{
    FIXUP_TARGET_TYPE_SEGMENT,
    FIXUP_TARGET_TYPE_EXTERN
};

typedef int FIXUP_TARGET_TYPE;

class EXPORT FIXUP_TARGET
{
public:
    FIXUP_TARGET();
    virtual ~FIXUP_TARGET();
    virtual FIXUP_TARGET_TYPE getType() = 0;
    virtual std::string getTypeAsString() = 0;
};

class EXPORT FIXUP_TARGET_SEGMENT : public FIXUP_TARGET
{
public:
    FIXUP_TARGET_SEGMENT(std::shared_ptr<VirtualSegment> target_segment);
    virtual ~FIXUP_TARGET_SEGMENT();
    std::shared_ptr<VirtualSegment> getTargetSegment();
    virtual FIXUP_TARGET_TYPE getType();
    virtual std::string getTypeAsString();
private:
    std::shared_ptr<VirtualSegment> target_segment;
};

class EXPORT FIXUP_TARGET_EXTERN : public FIXUP_TARGET
{
public:
    FIXUP_TARGET_EXTERN(std::string extern_name);
    virtual ~FIXUP_TARGET_EXTERN();
    std::string getExternalName();
    virtual FIXUP_TARGET_TYPE getType();
    virtual std::string getTypeAsString();
private:
    std::string extern_name;
};

class EXPORT FIXUP
{
public:
    FIXUP(std::shared_ptr<VirtualSegment> segment_to_fix, std::shared_ptr<FIXUP_TARGET> target, FIXUP_TYPE fixup_type, int offset, FIXUP_LENGTH length);
    virtual ~FIXUP();

    void setOffset(int offset);
    void appendOffset(int offset);
    int getOffset();
    FIXUP_LENGTH getLength();
    std::shared_ptr<VirtualSegment> getSegmentToFix();

    FIXUP_TYPE getType();
    std::string getTypeAsString();
    std::shared_ptr<FIXUP_TARGET> getTarget();

private:
    std::shared_ptr<VirtualSegment> segment_to_fix;
    std::shared_ptr<FIXUP_TARGET> target;
    int offset;
    FIXUP_TYPE type;
    FIXUP_LENGTH length;
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
    VirtualSegment(std::string segment_name, uint32_t origin);
    virtual ~VirtualSegment();
    std::string getName();
    std::shared_ptr<Stream> getStream();

    void register_fixup(std::shared_ptr<FIXUP_TARGET> fixup_target, FIXUP_TYPE fixup_type, int offset, FIXUP_LENGTH length);
    void register_fixup_target_segment(FIXUP_TYPE fixup_type, std::shared_ptr<VirtualSegment> relating_segment, int offset, FIXUP_LENGTH length);
    void register_fixup_target_extern(FIXUP_TYPE fixup_type, std::string extern_name, int offset, FIXUP_LENGTH length);
    void register_global_reference(std::string ref_name, int offset);
    std::vector<std::shared_ptr<FIXUP>> getFixups();
    bool hasFixups();

    std::vector<std::shared_ptr<GLOBAL_REF>> getGlobalReferences();
    std::shared_ptr<GLOBAL_REF> getGlobalReferenceByName(std::string ref_name);

    bool hasGlobalReferences();
    bool hasGlobalReference(std::string ref_name);

    bool hasOrigin();
    uint32_t getOrigin();

private:
    std::vector<std::shared_ptr<FIXUP>> fixups;
    std::vector<std::shared_ptr<GLOBAL_REF>> global_references;
    std::string segment_name;
    std::shared_ptr<Stream> stream;

    uint32_t origin;
};

#endif /* VIRTUALSEGMENT_H */

