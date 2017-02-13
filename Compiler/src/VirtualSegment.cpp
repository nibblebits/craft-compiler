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
 * File:   VirtualSegment.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 15:48
 * 
 * Description: 
 */

#include "VirtualSegment.h"

FIXUP_TARGET::FIXUP_TARGET()
{
    
}

FIXUP_TARGET::~FIXUP_TARGET()
{

}

FIXUP_TARGET_SEGMENT::FIXUP_TARGET_SEGMENT(std::shared_ptr<VirtualSegment> target_segment)
{
    this->target_segment = target_segment;
}

FIXUP_TARGET_SEGMENT::~FIXUP_TARGET_SEGMENT()
{

}

std::shared_ptr<VirtualSegment> FIXUP_TARGET_SEGMENT::getTargetSegment()
{
    return this->target_segment;
}

FIXUP_TARGET_TYPE FIXUP_TARGET_SEGMENT::getType()
{
    return FIXUP_TARGET_TYPE_SEGMENT;
}

std::string FIXUP_TARGET_SEGMENT::getTypeAsString()
{
    return "FIXUP_TARGET_TYPE_SEGMENT";
}

FIXUP_TARGET_EXTERN::FIXUP_TARGET_EXTERN(std::string extern_name)
{
    this->extern_name = extern_name;
}

FIXUP_TARGET_EXTERN::~FIXUP_TARGET_EXTERN()
{

}

std::string FIXUP_TARGET_EXTERN::getExternalName()
{
    return this->extern_name;
}

FIXUP_TARGET_TYPE FIXUP_TARGET_EXTERN::getType()
{
    return FIXUP_TARGET_TYPE_EXTERN;
}

std::string FIXUP_TARGET_EXTERN::getTypeAsString()
{
    return "FIXUP_TARGET_TYPE_EXTERN";
}

FIXUP::FIXUP(std::shared_ptr<VirtualSegment> segment_to_fix, std::shared_ptr<FIXUP_TARGET> target, FIXUP_TYPE fixup_type, int offset, FIXUP_LENGTH length)
{
    this->segment_to_fix = segment_to_fix;
    this->target = target;
    this->type = fixup_type;
    this->offset = offset;
    this->length = length;
}

FIXUP::~FIXUP()
{

}

void FIXUP::setOffset(int offset)
{
    this->offset = offset;
}

void FIXUP::appendOffset(int offset)
{
    this->offset += offset;
}

int FIXUP::getOffset()
{
    return this->offset;
}

FIXUP_LENGTH FIXUP::getLength()
{
    return this->length;
}

std::shared_ptr<VirtualSegment> FIXUP::getSegmentToFix()
{
    return this->segment_to_fix;
}

FIXUP_TYPE FIXUP::getType()
{
    return this->type;
}

std::string FIXUP::getTypeAsString()
{
    std::string str = "";
    switch (getType())
    {
    case FIXUP_TYPE_SELF_RELATIVE:
        str = "FIXUP_TYPE_SELF_RELATIVE";
        break;
    case FIXUP_TYPE_SEGMENT:
        str = "FIXUP_TYPE_SEGMENT";
        break;
    }

    return str;
}

std::shared_ptr<FIXUP_TARGET> FIXUP::getTarget()
{
    return this->target;
}

GLOBAL_REF::GLOBAL_REF(std::shared_ptr<VirtualSegment> segment, std::string ref_name, int offset)
{
    this->segment = segment;
    this->ref_name = ref_name;
    this->offset = offset;
}

GLOBAL_REF::~GLOBAL_REF()
{

}

std::shared_ptr<VirtualSegment> GLOBAL_REF::getSegment()
{
    return this->segment;
}

std::string GLOBAL_REF::getName()
{
    return this->ref_name;
}

int GLOBAL_REF::getOffset()
{
    return this->offset;
}

VirtualSegment::VirtualSegment(std::string segment_name, uint32_t origin)
{
    this->segment_name = segment_name;
    this->origin = origin;
    
    this->stream = std::shared_ptr<Stream>(new Stream());
}

VirtualSegment::~VirtualSegment()
{
}

std::string VirtualSegment::getName()
{
    return this->segment_name;
}

std::shared_ptr<Stream> VirtualSegment::getStream()
{
    return this->stream;
}

void VirtualSegment::register_fixup(std::shared_ptr<FIXUP_TARGET> fixup_target, FIXUP_TYPE fixup_type, int offset, FIXUP_LENGTH length)
{
    std::shared_ptr<FIXUP> fixup = std::shared_ptr<FIXUP>(new FIXUP(shared_from_this(), fixup_target, fixup_type, offset, length));
    this->fixups.push_back(fixup);
}

void VirtualSegment::register_fixup_target_segment(FIXUP_TYPE fixup_type, std::shared_ptr<VirtualSegment> relating_segment, int offset, FIXUP_LENGTH length)
{
    std::shared_ptr<FIXUP_TARGET_SEGMENT> target = std::shared_ptr<FIXUP_TARGET_SEGMENT>(new FIXUP_TARGET_SEGMENT(relating_segment));
    register_fixup(target, fixup_type, offset, length);
}

void VirtualSegment::register_fixup_target_extern(FIXUP_TYPE fixup_type, std::string extern_name, int offset, FIXUP_LENGTH length)
{
    std::shared_ptr<FIXUP_TARGET_EXTERN> target = std::shared_ptr<FIXUP_TARGET_EXTERN>(new FIXUP_TARGET_EXTERN(extern_name));
    register_fixup(target, fixup_type, offset, length);
}

void VirtualSegment::register_global_reference(std::string ref_name, int offset)
{
    std::shared_ptr<GLOBAL_REF> global_reference = std::shared_ptr<GLOBAL_REF>(new GLOBAL_REF(shared_from_this(), ref_name, offset));
    this->global_references.push_back(global_reference);
}

std::vector<std::shared_ptr<FIXUP>> VirtualSegment::getFixups()
{

    return this->fixups;
}

bool VirtualSegment::hasFixups()
{

    return !this->fixups.empty();
}

std::vector<std::shared_ptr<GLOBAL_REF>> VirtualSegment::getGlobalReferences()
{

    return this->global_references;
}

std::shared_ptr<GLOBAL_REF> VirtualSegment::getGlobalReferenceByName(std::string ref_name)
{
    for (std::shared_ptr<GLOBAL_REF> global_ref : this->global_references)
    {
        if (global_ref->getName() == ref_name)

            return global_ref;
    }

    return NULL;
}

bool VirtualSegment::hasGlobalReferences()
{

    return !getGlobalReferences().empty();
}

bool VirtualSegment::hasGlobalReference(std::string ref_name)
{
    for (std::shared_ptr<GLOBAL_REF> global_ref : this->global_references)
    {
        if (global_ref->getName() == ref_name)

            return true;
    }

    return false;
}

bool VirtualSegment::hasOrigin()
{

    return this->origin != 0;
}

uint32_t VirtualSegment::getOrigin()
{
    return this->origin;
}