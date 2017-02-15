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

#include <string>

#include "OMFObjectFormat.h"
#include "Compiler.h"

OMFObjectFormat::OMFObjectFormat(Compiler* compiler) : VirtualObjectFormat(compiler)
{
}

OMFObjectFormat::~OMFObjectFormat()
{
}

std::shared_ptr<VirtualSegment> OMFObjectFormat::new_segment(std::string segment_name, uint32_t origin)
{
    // We are not doing anything special so lets just return the standard virtual segment
    return std::shared_ptr<VirtualSegment>(new VirtualSegment(segment_name, origin));
}


LOCATION_TYPE OMFObjectFormat::get_location_type_from_fixup(std::shared_ptr<FIXUP> fixup)
{
    LOCATION_TYPE location_type;
    FIXUP_LENGTH length = fixup->getLength();
    if (length == FIXUP_16BIT)
    {
        location_type = FIXUPP_LOCATION_16_BIT_OFFSET;
    }
    else if (length == FIXUP_8BIT)
    {
        location_type = FIXUPP_LOCATION_LOW_ORDER_BYTE_8_BIT_DISPLACEMENT;
    }
    else
    {
        throw Exception("invalid or unimplemented fix up length for the OMF object file", "LOCATION_TYPE OMFObjectFormat::get_location_type_from_fixup(std::shared_ptr<FIXUP> fixup)");
    }

    return location_type;
}

void OMFObjectFormat::handle_segment_fixup(struct RECORD* record, std::shared_ptr<FIXUP> fixup, std::shared_ptr<FIXUP_TARGET_SEGMENT> fixup_target_seg)
{
    LOCATION_TYPE location_type = get_location_type_from_fixup(fixup);
    MagicOMFAddFIXUP16_SubRecord_Fixup_Internal(record,
                                                fixup_target_seg->getTargetSegment()->getName().c_str(),
                                                fixup->getOffset(),
                                                location_type);
}

void OMFObjectFormat::handle_extern_fixup(struct RECORD* record, std::shared_ptr<FIXUP> fixup, std::shared_ptr<FIXUP_TARGET_EXTERN> fixup_target_extern)
{
    LOCATION_TYPE location_type = get_location_type_from_fixup(fixup);
    MagicOMFAddFIXUP16_SubRecord_Fixup_External(record,
                                                fixup_target_extern->getExternalName().c_str(),
                                                fixup->getOffset(),
                                                location_type);
}

void OMFObjectFormat::read(std::shared_ptr<Stream> input_stream)
{
    
    char* buf = input_stream->getBuf();
    struct MagicOMFHandle* handle = MagicOMFTranslate(buf, input_stream->getSize(), true);
    if (handle->has_error)
    {
        throw Exception("Problem reading OMF file: " + std::string(GetErrorMessage(handle->last_error_code))
                        , "void OMFObjectFormat::read(std::shared_ptr<Stream> input_stream)");
    }

    struct RECORD* current = handle->root;
    while (current != NULL)
    {
        switch (current->type)
        {
        case SEGDEF_16_ID:
        {
            struct SEGDEF_16* segdef_16 = (struct SEGDEF_16*) current->contents;
            VirtualObjectFormat::createSegment(segdef_16->class_name_str);
        }
            break;
        case LEDATA_16_ID:
        {
            struct LEDATA_16* ledata_16 = (struct LEDATA_16*) current->contents;
            std::shared_ptr<VirtualSegment> segment = VirtualObjectFormat::getSegment(ledata_16->SEGDEF_16_record->class_name_str);
            std::shared_ptr<Stream> stream = segment->getStream();
            char* buf = ledata_16->data_bytes;
            for (int i = 0; i < ledata_16->data_bytes_size; i++)
            {
                stream->write8(buf[i]);
            }
        }
            break;
        case EXTDEF_ID:
        {
            struct EXTDEF* extdef = (struct EXTDEF*) current->contents;
            VirtualObjectFormat::registerExternalReference(extdef->name_str);
        }
            break;
        case PUBDEF_16_ID:
        {
            struct PUBDEF_16* pubdef_16 = (struct PUBDEF_16*) current->contents;
            std::shared_ptr<VirtualSegment> segment = VirtualObjectFormat::getSegment(pubdef_16->segdef_16_record->class_name_str);
            struct PUBDEF_16_IDEN* current_iden = pubdef_16->iden;
            while (current_iden != NULL)
            {
                VirtualObjectFormat::registerGlobalReference(segment, current_iden->name_str, current_iden->p_offset);
                current_iden = current_iden->next;
                break;
            }
        }
            break;
        case FIXUPP_16_ID:
        {
            struct FIXUP_16_SUBRECORD_DESCRIPTOR* fixup_16_desc = (struct FIXUP_16_SUBRECORD_DESCRIPTOR*) current->contents;
            while (fixup_16_desc != NULL)
            {
                if (fixup_16_desc->subrecord_type == FIXUPP_FIXUP_SUBRECORD)
                {
                    struct FIXUPP_16_FIXUP_SUBRECORD* subrecord = (struct FIXUPP_16_FIXUP_SUBRECORD*) fixup_16_desc->subrecord;
                    std::shared_ptr<VirtualSegment> target_segment = VirtualObjectFormat::getSegment(subrecord->target_data->SEGDEF_16_record->class_name_str);
                    FIXUP_LENGTH length;
                    if (subrecord->location == FIXUPP_LOCATION_LOW_ORDER_BYTE_8_BIT_DISPLACEMENT)
                    {
                        length = FIXUP_8BIT;
                    }
                    else if (subrecord->location == FIXUPP_LOCATION_16_BIT_OFFSET)
                    {
                        length = FIXUP_16BIT;
                    }
                    else
                    {
                        throw Exception("Unsupported location provided in OMF object file for the FIXUPP record.", "void OMFObjectFormat::read(std::shared_ptr<Stream> input_stream)");
                    }

                    FIXUP_TYPE fixup_type;
                    
                    if (subrecord->mode == FIXUPP_MODE_SELF_RELATIVE_FIXUP)
                    {
                        fixup_type = FIXUP_TYPE_SELF_RELATIVE;
                    }
                    else
                    {
                        fixup_type = FIXUP_TYPE_SEGMENT;
                    }
                    
                    if (subrecord->target_type == FIXUPP_TARGET_TYPE_EXTIDX)
                    {
                        // External fixup
                        target_segment->register_fixup_target_extern(fixup_type, subrecord->relating_extdef->name_str, subrecord->data_record_offset, length);
                    }
                    else if(subrecord->target_type == FIXUPP_TARGET_TYPE_SEGIDX)
                    {
                        // Internal fixup
                        std::shared_ptr<VirtualSegment> relating_segment = VirtualObjectFormat::getSegment(subrecord->relating_data->SEGDEF_16_record->class_name_str);
                        target_segment->register_fixup_target_segment(fixup_type, relating_segment, subrecord->data_record_offset, length);
                    }
                }
                fixup_16_desc = fixup_16_desc->next_subrecord_descriptor;
            }
        }
            break;

        }
        current = current->next;
    }
     
}

void OMFObjectFormat::finalize()
{
    
    struct RECORD* record;
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
    record = MagicOMFNewLNAMESRecord(handle);
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

    // We should write any pubdef records for global definitions
    for (std::shared_ptr<VirtualSegment> segment : getSegments())
    {
        if (segment->hasGlobalReferences())
        {
            struct RECORD* record = MagicOMFNewPUBDEF16Record(handle, segment->getName().c_str());
            for (std::shared_ptr<GLOBAL_REF> global_ref : segment->getGlobalReferences())
            {
                MagicOMFAddPUBDEF16Identifier(record, global_ref->getName().c_str(), global_ref->getOffset(), 0);
            }
            MagicOMFFinishPUBDEF16(record);
        }

    }

    // Write any external definition records
    if (hasExternalReferences())
    {
        record = MagicOMFNewEXTDEFRecord(handle);
        for (std::string external_ref : getExternalReferences())
        {
            MagicOMFAddEXTDEF(record, external_ref.c_str(), 0);
        }
        MagicOMFFinishEXTDEF(record);
    }

    // Now we need to create the LEDATA records
    for (std::shared_ptr<VirtualSegment> segment : getSegments())
    {
        std::shared_ptr<Stream> stream = segment->getStream();
        MagicOMFAddLEDATA16(handle, segment->getName().c_str(), 0, stream->getSize(), stream->getBuf());
        // Do we have any fixups for this segment?
        if (segment->hasFixups())
        {
            struct RECORD* record = MagicOMFNewFIXUP16Record(handle);
            for (std::shared_ptr<FIXUP> fixup : segment->getFixups())
            {
                std::shared_ptr<FIXUP_TARGET> fixup_target = fixup->getTarget();
                switch (fixup_target->getType())
                {
                case FIXUP_TARGET_TYPE_SEGMENT:
                    handle_segment_fixup(record, fixup, std::dynamic_pointer_cast<FIXUP_TARGET_SEGMENT>(fixup_target));
                    break;
                case FIXUP_TARGET_TYPE_EXTERN:
                    handle_extern_fixup(record, fixup, std::dynamic_pointer_cast<FIXUP_TARGET_EXTERN>(fixup_target));
                    break;
                }
            }
            MagicOMFFinishFIXUP16(record);
        }
    }

    // Finally we need to generate the MODEND record to signify the end of this object file
    MagicOMFAddMODEND16(handle);

    // Let us build the buffer
    MagicOMFGenerateBuffer(handle);

    // We now have the OMF object in the handles buffer
    for (int i = 0; i < handle->buf_size; i++)
    {
        getObjectStream()->write8(handle->buf[i]);
    }

}