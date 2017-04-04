/*
    Magic OMF Library v1.0 - A linker for the OMF(Object Model Format).
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

#include "translate.h"
#include "record.h"
#include "IO.h"
#include "MagicOMF.h"
#include "builder.h"
#include <stdlib.h>

/* 
 * File:   translate.c
 * Author: Daniel McCarthy
 *
 * Created on 06 December 2016, 11:52
 */


struct RECORD* StartRecord(struct MagicOMFHandle* handle)
{
    // Read the standard record details
    uint8 type = ReadUnsignedByte(&handle->next);
    uint16 length = ReadUnsignedWord(&handle->next);
    return BuildRecord(handle, type, length, 0);
}

void EndRecord(struct RECORD* record, struct MagicOMFHandle* handle)
{
    // Do we have a checksum? if so lets read it
    if (record->has_checksum)
    {
        record->checksum = ReadUnsignedByte(&handle->next);
    }

    // Add the record to the handle
    MagicOMFAddRecord(record);
}

void TranslatorSkipRecord(struct MagicOMFHandle* handle)
{
    ReadUnsignedByte(&handle->next);
    uint16 size = ReadUnsignedWord(&handle->next);

    // Now skip the records contents
    ReadAndIgnoreBytes(&handle->next, handle->next + size);
}

void TranslatorReadTHEADR(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != THEADR_ID)
    {
        error(INVALID_THEADR_PROVIDED, handle);
        return;
    }

    int name_size = ReadUnsignedByte(&handle->next);
    char* name_str = ReadStringAddTerminator(&handle->next, name_size);
    record->contents = BuildTHEADR_DefinedSize(name_str, name_size);
    EndRecord(record, handle);

}

void TranslatorReadLHEADR(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != LHEADR_ID)
    {
        error(INVALID_LHEADR_PROVIDED, handle);
        return;
    }

    struct LHEADR* contents = malloc(sizeof (struct LHEADR));
    record->contents = contents;
    contents->string_length = ReadUnsignedByte(&handle->next);
    contents->name_string = ReadStringAddTerminator(&handle->next, contents->string_length);
    EndRecord(record, handle);
}

void TranslatorReadCOMENT_Translator(struct RECORD* record, struct MagicOMFHandle* handle)
{
    struct COMENT* contents = (struct COMENT*) (record->contents);
    contents->c_string = ReadStringUntilEndAddTerminator(&handle->next, record->end_of_record);
}

void TranslatorReadCOMENT_LinkPassSeparator(struct RECORD* record, struct MagicOMFHandle* handle)
{
    struct COMENT* contents = (struct COMENT*) (record->contents);
    uint8 subtype = ReadUnsignedByte(&handle->next);
    if (subtype != 1)
    {
        // This is a problem it should be 1.
        // Show an error here.
    }
    else
    {
        contents->is_link_pass_seperator = true;
        // Specification states that its possible bytes can come after the subtype but they must be ignored
        ReadStringUntilEndAddTerminator(&handle->next, record->end_of_record);
    }
}

void TranslatorReadCOMENT(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != COMENT_ID)
    {
        error(INVALID_COMENT_PROVIDED, handle);
        return;
    }

    struct COMENT* contents = malloc(sizeof (struct COMENT));
    record->contents = contents;
    contents->c_type = ReadUnsignedByte(&handle->next);
    contents->c_class = ReadUnsignedByte(&handle->next);
    contents->c_string = NULL;

    switch (contents->c_class)
    {
    case COMENT_CLASS_LINK_PASS_SEPARATOR:
        TranslatorReadCOMENT_LinkPassSeparator(record, handle);
        break;
    case COMENT_CLASS_TRANSLATOR:
        TranslatorReadCOMENT_Translator(record, handle);
        break;
    default:
        error(UNKNOWN_COMENT_CLASS, handle);
    }


    contents->no_purge = (contents->c_type & 0x80) == 0x80;
    contents->no_list = (contents->c_type & 0x40) == 0x40;
    EndRecord(record, handle);

}

void TranslatorReadLNAMES(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != LNAMES_ID)
    {
        error(INVALID_LNAMES_PROVIDED, handle);
        return;
    }

    struct LNAMES* prev = NULL;
    struct LNAMES* root_lnames = NULL;
    while (handle->next < record->end_of_record)
    {
        struct LNAMES* contents = malloc(sizeof (struct LNAMES));
        contents->s_len = ReadUnsignedByte(&handle->next);
        contents->n_string = ReadStringAddTerminator(&handle->next, contents->s_len);
        contents->next = NULL;

        if (root_lnames == NULL)
        {
            // This must be the root
            root_lnames = contents;
        }
        else if (prev->next == NULL)
        {
            prev->next = contents;
        }
        prev = contents;
    }

    record->contents = root_lnames;
    EndRecord(record, handle);

}

/* 32 bit is not currently supported, translator will read from lowest possible
 * possibility. */
void TranslatorReadSEGDEF16(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != SEGDEF_16_ID)
    {
        error(INVALID_SEGDEF_16_PROVIDED, handle);
        return;
    }

    unsigned char c = *record->end_of_record;
    struct SEGDEF_16* contents = malloc(sizeof (struct SEGDEF_16));
    record->contents = contents;
    uint8 ACBP = ReadUnsignedByte(&handle->next);
    uint8 A = ACBP >> 5;
    uint8 C = (ACBP >> 2) & 0x07;
    uint8 B = (ACBP >> 1) & 0x01;
    uint8 P = (ACBP & 0x01);
    contents->attributes.A = A;
    contents->attributes.C = C;
    contents->attributes.B = B;
    contents->attributes.P = P;
    contents->seg_len = ReadUnsignedWord(&handle->next);

    // When B = 1 then the segment length must also be zero.
    if (B == 1)
    {
        if (!contents->seg_len != 0)
        {
            error(IMPROPERLY_FORMATTED_SEGDEF_16_PROVIDED, handle);
            return;
        }

        // Seg length is 64KB 
        contents->seg_len = 0xffff;
    }

    contents->seg_name_index = ReadUnsignedByte(&handle->next);
    contents->class_name_index = ReadUnsignedByte(&handle->next);
    contents->overlay_name_index = ReadUnsignedByte(&handle->next);

    // Lets set the LNAME string that this SEGDEF_16 is using just for ease of access
    contents->class_name_str = MagicOMFGetLNAMESNameByIndex(handle, contents->seg_name_index);
    if (contents->class_name_str == NULL)
    {
        // The file doesn't define an LNAME with the index specified...
        error(LNAMES_NOT_FOUND, handle);
    }
    EndRecord(record, handle);

}

void TranslatorReadPUBDEF16(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != PUBDEF_16_ID)
    {
        error(INVALID_PUBDEF_16_PROVIDED, handle);
    }

    struct PUBDEF_16* contents = malloc(sizeof (struct PUBDEF_16));
    record->contents = contents;
    contents->bg_index = ReadUnsignedByte(&handle->next);
    contents->bs_index = ReadUnsignedByte(&handle->next);
    contents->segdef_16_record = MagicOMFGetSEGDEF16ByIndex(handle, contents->bs_index);
    if (contents->bs_index == 0)
    {
        if (contents->bg_index == 0)
        {
            // Absolute addressing is not supported
            error(PUBDEF_16_ABSOLUTE_ADDRESSING_NOT_SUPPORTED, handle);
        }
        else
        {
            // Ok this is legal but we do not care about the base frame so just read but ignore it
            ReadUnsignedWord(&handle->next);
        }
    }

    struct PUBDEF_16_IDEN* prev = NULL;
    while (handle->next < record->end_of_record)
    {
        struct PUBDEF_16_IDEN* iden = malloc(sizeof (struct PUBDEF_16_IDEN));
        iden->str_len = ReadUnsignedByte(&handle->next);
        iden->name_str = ReadStringAddTerminator(&handle->next, iden->str_len);
        iden->p_offset = ReadUnsignedWord(&handle->next);
        iden->type_index = ReadUnsignedByte(&handle->next);
        if (prev == NULL)
        {
            // This is the first iteration so set the contents iden pointer
            contents->iden = iden;
        }
        else
        {
            // Set the previous identifier to point to us
            prev->next = iden;
        }
        iden->next = NULL;
        prev = iden;
    }

    EndRecord(record, handle);

}

void TranslatorReadLEDATA16(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != LEDATA_16_ID)
    {
        error(INVALID_LEDATA_16_PROVIDED, handle);
    }

    struct LEDATA_16* contents = malloc(sizeof (struct LEDATA_16));
    contents->seg_index = ReadUnsignedByte(&handle->next);
    if (contents->seg_index == 0)
    {
        // Seg index must be non zero!
        error(INVALID_LEDATA_16_PROVIDED, handle);
    }

    contents->data_offset = ReadUnsignedWord(&handle->next);

    // -4 due to seg index, data offset and checksum.
    contents->data_bytes_size = record->length - 4;
    if (contents->data_bytes_size > 1024)
    {
        // Specification prohibits the data bytes size being above 1024
        error(INVALID_LEDATA_16_PROVIDED, handle);
    }


    contents->data_bytes = ReadDataUntilEnd(&handle->next, record->end_of_record);
    contents->SEGDEF_16_record = MagicOMFGetSEGDEF16ByIndex(handle, contents->seg_index);

    record->contents = contents;
    EndRecord(record, handle);

    handle->last_ledata = contents;
}

void TranslatorReadFIXUPP16(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != FIXUPP_16_ID)
    {
        error(INVALID_FIXUPP_16_PROVIDED, handle);
    }

    struct FIXUP_16_SUBRECORD_DESCRIPTOR* root_subrecord_desc = NULL;
    struct FIXUP_16_SUBRECORD_DESCRIPTOR* prev_subrecord_desc = NULL;
    struct FIXUP_16_SUBRECORD_DESCRIPTOR* subrecord_desc = NULL;
    while (handle->next < record->end_of_record)
    {
        subrecord_desc = malloc(sizeof (struct FIXUP_16_SUBRECORD_DESCRIPTOR));
        subrecord_desc->subrecord = NULL;
        subrecord_desc->next_subrecord_descriptor = NULL;
        if (prev_subrecord_desc != NULL)
        {
            prev_subrecord_desc->next_subrecord_descriptor = subrecord_desc;
        }

        if (root_subrecord_desc == NULL)
        {
            root_subrecord_desc = subrecord_desc;
        }

        uint8 thread_or_fixup = ReadUnsignedByte(&handle->next);
        if (thread_or_fixup & 0x80)
        {
            subrecord_desc->subrecord_type = FIXUPP_FIXUP_SUBRECORD;
            // Ok this is a FIXUP
            // Build the locat
            uint16 locat = (thread_or_fixup << 8 | ReadUnsignedByte(&handle->next));
            TranslatorReadFIXUPP16_FIXUP_SUBRECORD(locat, subrecord_desc, handle);
        }
        else
        {
            // THREAD not supported
            error(FIXUPP_16_THREAD_NOT_SUPPORTED, handle);
            return;
        }

        prev_subrecord_desc = subrecord_desc;
    }

    record->contents = root_subrecord_desc;
    EndRecord(record, handle);
}

void TranslatorReadFIXUPP16_FIXUP_SUBRECORD(uint16 locat, struct FIXUP_16_SUBRECORD_DESCRIPTOR* subrecord_descriptor, struct MagicOMFHandle* handle)
{
    struct FIXUPP_16_FIXUP_SUBRECORD* subrecord = malloc(sizeof (struct FIXUPP_16_FIXUP_SUBRECORD));
    subrecord->mode = (locat >> 14) & 0x01;
    subrecord->location = (locat >> 10) & 0x0f;
    subrecord->data_record_offset = (locat) & 0x3ff;

    // Set the absolute offset, relative offset + absolute offset to last LEDATA record
    subrecord->abs_data_record_offset = subrecord->data_record_offset + handle->last_ledata->data_offset;

    // Ok only 16 bit offsets are currently supported
    if (subrecord->location != FIXUPP_LOCATION_16_BIT_OFFSET)
    {
        error(FIXUPP_16_LOCATION_NOT_SUPPORTED, handle);
        return;
    }

    uint8 fix_data = ReadUnsignedByte(&handle->next);
    uint8 F = (fix_data >> 7) & 0x01;
    uint8 frame = (fix_data >> 4) & 0x07;
    uint8 T = (fix_data >> 3) & 0x01;
    uint8 P = (fix_data >> 2) & 0x01;
    uint8 targt = (fix_data) & 0x03;

    subrecord->fix_data = fix_data;

    /* F <= 2 is a guess as spec states: The Frame Datum field is present
and is an index field for FRAME methods F0, F1, and F2 only. But binary appears that
     frame datum field is not present unless the frame is F0, F1 or F2.*/

    subrecord->has_frame_datum = (F == 0 && frame <= 2);
    subrecord->has_target_displacement = !(T == 0 || T == 1 && P == 1);

    // We can't handle TARGET threads they are unsupported, we also only support frame: TARGET.
    if (T != 0 || frame != 0x05)
    {
        error(FIXUPP_16_FIX_DATA_UNSUPPORTED, handle);
        return;
    }

    subrecord->target_type = targt;

    if (subrecord->has_frame_datum)
    {
        subrecord->frame_datum = ReadUnsignedByte(&handle->next);
    }

    subrecord->has_target_datum = true;
    subrecord->target_datum = ReadUnsignedByte(&handle->next);

    if (subrecord->has_target_displacement)
    {
        subrecord->target_displacement = ReadUnsignedWord(&handle->next);
    }

    subrecord_descriptor->subrecord = subrecord;
}

void TranslatorReadMODEND16(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != MODEND_16_ID)
    {
        error(INVALID_MODEND_16_PROVIDED, handle);
    }

    struct MODEND_16* contents = malloc(sizeof (struct MODEND_16));
    uint8 module_type = ReadUnsignedByte(&handle->next);
    if (module_type & 0x40)
    {
        // We don't support start addresses
        error(MODEND_16_MODULE_TYPE_UNSUPPORTED, handle);
    }

    contents->is_main = (module_type >> 6) & 0x01;
    contents->has_start_address = (module_type >> 7);

    EndRecord(record, handle);
}

void TranslatorReadEXTDEF(struct MagicOMFHandle* handle)
{
    struct RECORD* record = StartRecord(handle);
    if (record->type != EXTDEF_ID)
    {
        error(INVALID_EXTDEF_PROVIDED, handle);
    }

    struct EXTDEF* extdef_root_contents = malloc(sizeof (struct EXTDEF));
    record->contents = extdef_root_contents;

    struct EXTDEF* prev = NULL;
    struct EXTDEF* contents = extdef_root_contents;
    while (handle->next < record->end_of_record)
    {
        contents->s_len = ReadUnsignedByte(&handle->next);
        contents->name_str = ReadStringAddTerminator(&handle->next, contents->s_len);
        contents->type_index = ReadUnsignedByte(&handle->next);

        if (prev != NULL)
        {
            prev->next = contents;
        }

        prev = contents;

        contents = malloc(sizeof (struct EXTDEF));
    }
    EndRecord(record, handle);
}

void TranslatorFinalize(struct MagicOMFHandle* handle)
{
    struct RECORD* record = handle->root;
    while (record != NULL)
    {
        if (record->type == FIXUPP_16_ID)
        {
            TranslatorFinalize_FIXUPP_16(record, handle);
        }
        record = record->next;
    }
}

void TranslatorFinalize_FIXUPP_16(struct RECORD* record, struct MagicOMFHandle* handle)
{
    struct FIXUP_16_SUBRECORD_DESCRIPTOR* record_descriptor = (struct FIXUP_16_SUBRECORD_DESCRIPTOR*) (record->contents);
    while (record_descriptor != NULL)
    {
        if (record_descriptor->subrecord_type == FIXUPP_FIXUP_SUBRECORD)
        {
            struct FIXUPP_16_FIXUP_SUBRECORD* subrecord = (struct FIXUPP_16_FIXUP_SUBRECORD*) (record_descriptor->subrecord);
            if (record->prev == NULL || record->prev->type != LEDATA_16_ID)
            {
                // No LEDATA was found we are expecting it.
                error(BINARY_FORMATTING_PROBLEM, handle);
            }
            else
            {
                struct LEDATA_16* prev_ledata_16 = (struct LEDATA_16*) (record->prev->contents);
                // OMF specifies that the target data will be the previous LEDATA record
                subrecord->target_data = prev_ledata_16;

                if (subrecord->target_type == FIXUPP_TARGET_TYPE_EXTIDX)
                {
                    subrecord->relating_extdef = MagicOMFGetEXTDEFByIndex(handle, subrecord->target_datum);
                }
                else
                {
                    subrecord->relating_data = MagicOMFGetLEDATABySegmentIndex(handle, subrecord->target_datum);
                }
            }
            record_descriptor = record_descriptor->next_subrecord_descriptor;
        }
    }
}