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
 * File:   generator.c
 * Author: dansb
 *
 * Created on 12 January 2017, 03:24
 */

#include "generator.h"
#include "record.h"

void GeneratorWriteRecordHeader(char** ptr, struct RECORD* record)
{
    // Setup where we will write to
    WritingToPointer(ptr);
    // Write the opcode
    WriteUnsignedByte(record->type);
    // Write the record length
    WriteUnsignedWord(record->length);
}

void GeneratorWriteTHEADR(char** ptr, struct RECORD* record)
{
    if (record->type != THEADR_ID)
    {
        error(INVALID_THEADR_PROVIDED, record->handle);
        return;
    }

    struct THEADR* theadr = (struct THEADR*) record->contents;
    // Write the record header
    GeneratorWriteRecordHeader(ptr, record);
    // Write the string size
    WriteUnsignedByte(theadr->string_length);
    // Now write the string
    WriteStringNoTerminator(theadr->name_string, theadr->string_length);

    // Now the checksum
    WriteUnsignedByte(record->checksum);
}

void GeneratorWriteCOMENT(char** ptr, struct RECORD* record)
{
    if (record->type != COMENT_ID)
    {
        error(INVALID_COMENT_PROVIDED, record->handle);
        return;
    }

    // Write the record header
    GeneratorWriteRecordHeader(ptr, record);

    struct COMENT* coment = (struct COMENT*) record->contents;
    WriteUnsignedByte(coment->c_type);
    WriteUnsignedByte(coment->c_class);
    WriteData(coment->c_string, record->length - 3);

    // Now the checksum
    WriteUnsignedByte(record->checksum);
}

void GeneratorWriteLNAMES(char** ptr, struct RECORD* record)
{
    if (record->type != LNAMES_ID)
    {
        error(INVALID_LNAMES_PROVIDED, record->handle);
        return;
    }

    // Write the record header
    GeneratorWriteRecordHeader(ptr, record);

    struct LNAMES* lnames = (struct LNAMES*) record->contents;
    struct LNAMES* current = lnames;
    while (current != NULL)
    {
        WriteUnsignedByte(current->s_len);
        WriteData(current->n_string, current->s_len);
        current = current->next;
    }

    // Write the checksum
    WriteUnsignedByte(record->checksum);
}

void GeneratorWriteEXTDEF(char** ptr, struct RECORD* record)
{
    if (record->type != EXTDEF_ID)
    {
        error(INVALID_EXTDEF_PROVIDED, record->handle);
    }

    // Write the record header
    GeneratorWriteRecordHeader(ptr, record);

    struct EXTDEF* current = (struct EXTDEF*) record->contents;
    while (current != NULL)
    {
        // Write the string length
        WriteUnsignedByte(current->s_len);
        // Write the string
        WriteData(current->name_str, current->s_len);
        // Write the type index
        WriteUnsignedByte(current->type_index);
        // Get the next EXTDEF
        current = current->next;
    }

    WriteUnsignedByte(record->checksum);
}

void GeneratorWriteSEGDEF16(char** ptr, struct RECORD* record)
{
    if (record->type != SEGDEF_16_ID)
    {
        error(INVALID_SEGDEF_16_PROVIDED, record->handle);
        return;
    }

    // Write the record header
    GeneratorWriteRecordHeader(ptr, record);

    struct SEGDEF_16* segdef_16 = (struct SEGDEF_16*) record->contents;
    struct Attributes attributes = segdef_16->attributes;
    // Lets create the ACBP
    uint8 ACBP = (attributes.A << 5) | (attributes.C << 2) | (attributes.B << 1) | (attributes.P);
    // Write the ACBP
    WriteUnsignedByte(ACBP);
    if (attributes.A == SEG_ATTR_ALIGNMENT_ABS_SEG)
    {
        // Alignment is an absolute segment so we are expecting a frame number and an offset
        WriteUnsignedWord(attributes.frame_number);
        WriteUnsignedByte(attributes.offset);
    }

    if (attributes.B == 1)
    {
        WriteUnsignedWord(0);
    }
    else
    {
        WriteUnsignedWord(segdef_16->seg_len);
    }

    WriteUnsignedByte(segdef_16->seg_name_index);
    WriteUnsignedByte(segdef_16->class_name_index);
    WriteUnsignedByte(segdef_16->overlay_name_index);

    // Finally write the checksum
    WriteUnsignedByte(record->checksum);
}

void GeneratorWriteLEDATA16(char** ptr, struct RECORD* record)
{
    if (record->type != LEDATA_16_ID)
    {
        error(INVALID_LEDATA_16_PROVIDED, record->handle);
        return;
    }

    struct LEDATA_16* ledata_16 = (struct LEDATA_16*) record->contents;

    // Write the record header
    GeneratorWriteRecordHeader(ptr, record);

    WriteUnsignedByte(ledata_16->seg_index);
    WriteUnsignedWord(ledata_16->data_offset);

    // Write the data bytes to the stream
    WriteData(ledata_16->data_bytes, ledata_16->data_bytes_size);

    // Finally lets write the checksum
    WriteUnsignedByte(record->checksum);
}

void GeneratorWriteFIXUPP16(char** ptr, struct RECORD* record)
{
    if (record->type != FIXUPP_16_ID)
    {
        error(INVALID_FIXUPP_16_PROVIDED, record->handle);
        return;
    }

    // Write the record header
    GeneratorWriteRecordHeader(ptr, record);

    struct FIXUP_16_SUBRECORD_DESCRIPTOR* subrecord_desc = (struct FIXUP_16_SUBRECORD_DESCRIPTOR*) record->contents;
    while (subrecord_desc != NULL)
    {
        if (subrecord_desc->subrecord_type == FIXUPP_FIXUP_SUBRECORD)
        {
            // Only fixup subrecords are currently supported and it is limited support
            struct FIXUPP_16_FIXUP_SUBRECORD* subrecord = (struct FIXUPP_16_FIXUP_SUBRECORD*) subrecord_desc->subrecord;
            if (subrecord->data_record_offset > 1024)
            {
                error(FIXUPP_16_OFFSET_OUT_OF_BOUNDS, record->handle);
                return;
            }
            // We must construct the locat
            uint16 locat = (0x01 << 15) | (subrecord->mode << 14) | (subrecord->location << 10) | (subrecord->data_record_offset);
            // Write the locat
            WriteUnsignedByte(locat >> 8);
            WriteUnsignedByte(locat);

            // Write the fix data ( spec states conditional but not how )
            WriteUnsignedByte(subrecord->fix_data);

            // Do we need to write a frame datum?
            if (!(subrecord->fix_data & FIXUPP_FIXUP_SET_F))
            {
                // F is not set which means we have a frame datum
                WriteUnsignedByte(subrecord->frame_datum);
            }

            // Do we have a target displacement?
            if (!(subrecord->fix_data & FIXUPP_FIXUP_SET_P))
            {
                // P is not set which means we have a target displacement
                WriteUnsignedWord(subrecord->target_displacement);
            }

        }
        subrecord_desc = subrecord_desc->next_subrecord_descriptor;
    }
    // Finally write the checksum
    WriteUnsignedByte(record->checksum);
}

void GeneratorWritePUBDEF16(char** ptr, struct RECORD* record)
{
    struct PUBDEF_16* contents = (struct PUBDEF_16*) record->contents;
    // Write the record header
    GeneratorWriteRecordHeader(ptr, record);
    // Now comes record information
    WriteUnsignedByte(contents->bg_index);
    WriteUnsignedByte(contents->bs_index);
    // Do we need to write a base frame?
    if (contents->bs_index == 0)
    {
        // Yes lets just write a NULL word
        WriteUnsignedWord(0);
    }


    // Now comes pubdef 16 internals
    struct PUBDEF_16_IDEN* iden = contents->iden;
    while (iden != NULL)
    {
        WriteUnsignedByte(iden->str_len);
        WriteData(iden->name_str, iden->str_len);
        WriteUnsignedWord(iden->p_offset);
        WriteUnsignedByte(iden->type_index);
        iden = iden->next;
    }

    // Finally the checksum
    WriteUnsignedByte(record->checksum);
}

void GeneratorWriteMODEND16(char** ptr, struct RECORD* record)
{
    if (record->type != MODEND_16_ID)
    {
        error(INVALID_MODEND_16_PROVIDED, record->handle);
        return;
    }

    // Write the record header
    GeneratorWriteRecordHeader(ptr, record);

    // Write value of zero for module type
    WriteUnsignedByte(0);

    // Write the checksum
    WriteUnsignedByte(record->checksum);
}