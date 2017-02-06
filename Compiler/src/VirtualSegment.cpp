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

FIXUP::FIXUP()
{

}

FIXUP::~FIXUP()
{

}

FIXUP_STANDARD::FIXUP_STANDARD(int offset, FIXUP_LENGTH length)
{
    this->offset = offset;
    this->length = length;
}

FIXUP_STANDARD::~FIXUP_STANDARD()
{

}

void FIXUP_STANDARD::setOffset(int offset)
{
    this->offset = offset;
}

void FIXUP_STANDARD::appendOffset(int offset)
{
    this->offset += offset;
}

int FIXUP_STANDARD::getOffset()
{
    return this->offset;
}

FIXUP_LENGTH FIXUP_STANDARD::getFixupLength()
{
    return this->length;
}

SEGMENT_FIXUP::SEGMENT_FIXUP(std::shared_ptr<VirtualSegment> relating_segment, int offset, FIXUP_LENGTH length) : FIXUP_STANDARD(offset, length)
{
    this->relating_segment = relating_segment;
}

SEGMENT_FIXUP::~SEGMENT_FIXUP()
{

}

std::shared_ptr<VirtualSegment> SEGMENT_FIXUP::getRelatingSegment()
{
    return this->relating_segment;
}

FIXUP_TYPE SEGMENT_FIXUP::getType()
{
    return FIXUP_TYPE_SEGMENT;
}

EXTERN_FIXUP::EXTERN_FIXUP(std::string extern_name, int offset, FIXUP_LENGTH length) : FIXUP_STANDARD(offset, length)
{
    this->extern_name = extern_name;
}

EXTERN_FIXUP::~EXTERN_FIXUP()
{

}

std::string EXTERN_FIXUP::getExternalName()
{
    return this->extern_name;
}

FIXUP_TYPE EXTERN_FIXUP::getType()
{
    return FIXUP_TYPE_EXTERN;
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

VirtualSegment::VirtualSegment(std::string segment_name)
{
    this->segment_name = segment_name;
}

VirtualSegment::~VirtualSegment()
{
}

std::string VirtualSegment::getName()
{
    return this->segment_name;
}

Stream* VirtualSegment::getStream()
{
    return &this->stream;
}

void VirtualSegment::register_fixup(std::shared_ptr<VirtualSegment> relating_segment, int offset, FIXUP_LENGTH length)
{
    // Fixups are resolved during link time, we should register them for now
    std::shared_ptr<SEGMENT_FIXUP> fixup = std::shared_ptr<SEGMENT_FIXUP>(new SEGMENT_FIXUP(relating_segment, offset, length));
    this->fixups.push_back(fixup);
}

void VirtualSegment::register_fixup_extern(std::string extern_name, int offset, FIXUP_LENGTH length)
{
    std::shared_ptr<EXTERN_FIXUP> fixup = std::shared_ptr<EXTERN_FIXUP>(new EXTERN_FIXUP(extern_name, offset, length));
    this->fixups.push_back(fixup);
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

bool VirtualSegment::hasGlobalReferences()
{
    return !getGlobalReferences().empty();
}