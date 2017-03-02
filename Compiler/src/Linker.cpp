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
 * File:   Linker.cpp
 * Author: Daniel McCarthy
 *
 * Created on 02 July 2016, 03:20
 * 
 * Description: The base linker class for all linkers
 */

#include "Linker.h"
#include "Stream.h"
#include "VirtualObjectFormat.h"
#include "common.h"

Linker::Linker(Compiler* compiler) : CompilerEntity(compiler)
{
}

Linker::~Linker()
{

}

void Linker::addObjectFile(std::shared_ptr<VirtualObjectFormat> obj)
{
    if (hasObjectFile(obj))
    {
        throw Exception("The object file provided has already been added to this linker", "void Linker::addObjectFile(std::shared_ptr<VirtualObjectFormat> obj)");
    }
    this->obj_stack.push_back(obj);
}

bool Linker::hasObjectFile(std::shared_ptr<VirtualObjectFormat> obj)
{
    return std::find(this->obj_stack.begin(), this->obj_stack.end(), obj) != this->obj_stack.end();
}

void Linker::link()
{
    if (this->obj_stack.empty())
    {
        throw Exception("Nothing to link", "void Linker::link()");
    }
    std::shared_ptr<VirtualObjectFormat> main_obj = this->obj_stack.front();
    this->obj_stack.pop_front();
    while (!this->obj_stack.empty())
    {
        std::shared_ptr<VirtualObjectFormat> other_obj = this->obj_stack.front();
        this->obj_stack.pop_front();
        this->link_merge(main_obj, other_obj);
    }


    /* At this point we should have all external references defined in one object file, 
     * so lets make sure external references can be linked up properly */

    for (std::string ext_ref : main_obj->getExternalReferences())
    {
        if (!main_obj->hasGlobalReference(ext_ref))
        {
            throw Exception("Undefined reference: " + ext_ref + ", ensure that you are including all your object files and try again", "void Linker::link()");
        }
    }

    // Resolve unknown symbols
    this->resolve(main_obj);

#ifdef DEBUG_MODE
    std::cout << "Resolved linker Object" << std::endl;
    debug_virtual_object_format(main_obj);
#endif

    // Ok we may need to apply an origin for each segment now
    for (std::shared_ptr<VirtualSegment> segment : main_obj->getSegments())
    {
        for (std::shared_ptr<FIXUP> fixup : segment->getFixups())
        {
            if (fixup->getType() == FIXUP_TYPE_SEGMENT)
            {
                if (fixup->getTarget()->getType() == FIXUP_TARGET_TYPE_SEGMENT)
                {
                    std::shared_ptr<FIXUP_TARGET_SEGMENT> fixup_target_segment = std::dynamic_pointer_cast<FIXUP_TARGET_SEGMENT>(fixup->getTarget());
                    if (fixup_target_segment->getTargetSegment()->hasOrigin())
                    {
                        int origin = fixup_target_segment->getTargetSegment()->getOrigin();
                        int fixup_old_value;
                        int fixup_new_value;
                        switch (fixup->getLength())
                        {
                        case FIXUP_8BIT:
                            fixup_old_value = segment->getStream()->peek8(fixup->getOffset());
                            break;
                        case FIXUP_16BIT:
                            fixup_old_value = segment->getStream()->peek16(fixup->getOffset());
                            break;
                        case FIXUP_32BIT:
                            fixup_old_value = segment->getStream()->peek32(fixup->getOffset());
                            break;
                        }


                        fixup_new_value = fixup_old_value + origin;

                        // Now finally lets overwrite it with the new value
                        switch (fixup->getLength())
                        {
                        case FIXUP_8BIT:
                            segment->getStream()->overwrite8(fixup->getOffset(), fixup_new_value);
                            break;
                        case FIXUP_16BIT:
                            segment->getStream()->overwrite16(fixup->getOffset(), fixup_new_value);
                            break;
                        case FIXUP_32BIT:
                            segment->getStream()->overwrite32(fixup->getOffset(), fixup_new_value);
                            break;
                        }
                    }
                }
                else
                {
                    // May need to apply origin for external fixups here, really depends not implementing it yet 
                }
            }
        }
    }

#ifdef DEBUG_MODE
    std::cout << "Final Object" << std::endl;
    debug_virtual_object_format(main_obj);
#endif
    // Build the executable
    this->build(&this->executable_stream, main_obj);

}

void Linker::link_merge(std::shared_ptr<VirtualObjectFormat> obj1, std::shared_ptr<VirtualObjectFormat> obj2)
{
    // We need to merge the formats
    obj1->append(obj2);
}

Stream* Linker::getExecutableStream()
{
    return &this->executable_stream;
}