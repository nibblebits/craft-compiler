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
 * File:   builder.c
 * Author: dansb
 *
 * Created on 11 January 2017, 22:44
 */

#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "builder.h"
#include "MagicOMF.h"

struct RECORD* BuildRecord(struct MagicOMFHandle* handle, uint8 record_type, uint16 record_length, uint8 checksum)
{
    struct RECORD* record = (struct RECORD*) malloc(sizeof (struct RECORD));

    // Read the standard record details
    record->type = record_type;
    record->length = record_length;
    if (handle->next == NULL)
    {
        record->end_of_record = NULL;
    }
    else
    {
        // -1 so end of record will point to the checksum.
        record->end_of_record = handle->next + record->length - 1;
    }
    // Set the child pointer to NULL
    record->next = NULL;
    // Lets set the previous
    record->prev = handle->last;
    // Assume we have a checksum until otherwise specified
    record->has_checksum = true;
    record->contents = NULL;
    record->checksum = checksum;
    record->handle = handle;
    return record;
}

struct THEADR* BuildTHEADR(char* name)
{
    return BuildTHEADR_DefinedSize(name, strlen(name));
}

struct THEADR* BuildTHEADR_DefinedSize(char* name, int size)
{
    struct THEADR* contents = malloc(sizeof (struct THEADR));
    contents->string_length = size;
    contents->name_string = name;
    return contents;
}

struct COMENT* BuildCOMENT(COMMENT_TYPE type, uint8 _class, char* str)
{
    struct COMENT* contents = malloc(sizeof (struct COMENT));
    contents->c_type = type;
    contents->c_class = _class;
    contents->c_string = str;
    contents->no_list = type & 0x40;
    contents->no_purge = type & 0x80;
    if (contents->c_class == COMENT_CLASS_LINK_PASS_SEPARATOR)
    {
        contents->is_link_pass_seperator = true;
    }
    else
    {
        contents->is_link_pass_seperator = false;
    }

    return contents;
}

struct LNAMES* BuildLNAMES(char* name)
{
    struct LNAMES* contents = malloc(sizeof (struct LNAMES));
    contents->s_len = strlen(name);
    contents->n_string = name;
    contents->next = NULL;
    return contents;
}

struct EXTDEF* BuildEXTDEF(char* name, int type_index)
{
    struct EXTDEF* contents = malloc(sizeof (struct EXTDEF));
    contents->s_len = strlen(name);
    contents->name_str = name;
    contents->type_index = type_index;
    contents->next = NULL;
    return contents;
}

struct SEGDEF_16* BuildSEGDEF16(struct MagicOMFHandle* handle, const char* name, struct Attributes attributes, uint16 size)
{
    struct SEGDEF_16* segdef_16 = malloc(sizeof (struct SEGDEF_16));
    segdef_16->attributes = attributes;
    segdef_16->seg_len = size;
    segdef_16->class_name_index = MagicOMFGetLNAMESIndex(handle, name);
    if (segdef_16->class_name_index == -1)
    {
        error(LNAMES_NOT_FOUND, handle);
    }
    segdef_16->class_name_str = MagicOMFGetLNAMESNameByIndex(handle, segdef_16->class_name_index);
    segdef_16->overlay_name_index = segdef_16->class_name_index;
    segdef_16->seg_name_index = segdef_16->class_name_index;
    return segdef_16;
}

struct LEDATA_16* BuildLEDATA16(struct MagicOMFHandle* handle, const char* seg_name, uint16 data_offset, int data_size, char* data)
{
    uint8 seg_index = MagicOMFGetSEGDEFIndex(handle, seg_name);
    struct LEDATA_16* ledata_16 = malloc(sizeof (struct LEDATA_16));
    ledata_16->seg_index = seg_index;
    ledata_16->data_offset = data_offset;
    ledata_16->data_bytes_size = data_size;
    ledata_16->data_bytes = data;
    return ledata_16;
};

struct FIXUP_16_SUBRECORD_DESCRIPTOR* BuildFIXUP16_RecordDescriptor(uint8 subrecord_type, const void* subrecord)
{
    struct FIXUP_16_SUBRECORD_DESCRIPTOR* subrecord_desc = (struct FIXUP_16_SUBRECORD_DESCRIPTOR*) malloc(sizeof (struct FIXUP_16_SUBRECORD_DESCRIPTOR));
    subrecord_desc->subrecord_type = subrecord_type;
    subrecord_desc->subrecord = subrecord;
    subrecord_desc->next_subrecord_descriptor = NULL;
    return subrecord_desc;
}

// I think frame datum should actually be target datum in this case, check this out another time.

struct FIXUPP_16_FIXUP_SUBRECORD* BuildFIXUP16_SubRecord_Segment_Fixup(struct MagicOMFHandle* handle, const char* referring_to_segment_name, uint16 offset, LOCATION_TYPE location_type, FIXUP_MODE fixup_mode)
{
    // We need to get the index of the segment we are referring to
    int ref_seg = MagicOMFGetSEGDEFIndex(handle, referring_to_segment_name);

    struct FIXUPP_16_FIXUP_SUBRECORD* subrecord = (struct FIXUPP_16_FIXUP_SUBRECORD*) malloc(sizeof (struct FIXUPP_16_FIXUP_SUBRECORD));
    // Frame datum should hold the index of the segment we are referring to.
    subrecord->frame_datum = ref_seg;

    subrecord->mode = fixup_mode;
    subrecord->location = location_type;

    subrecord->data_record_offset = offset;

    // Just a default for now, I don't know enough about it to do it properly.
    subrecord->fix_data = 0x54;

    return subrecord;

}

// I think frame datum should actually be target datum in this case, check this out another time.

struct FIXUPP_16_FIXUP_SUBRECORD* BuildFIXUP16_SubRecord_External_Fixup(struct MagicOMFHandle* handle, const char* extern_ref_name, uint16 offset, LOCATION_TYPE location_type, FIXUP_MODE fixup_mode)
{
    int ref_extern = MagicOMFGetEXTDEFIndex(handle, extern_ref_name);
    struct FIXUPP_16_FIXUP_SUBRECORD* subrecord = (struct FIXUPP_16_FIXUP_SUBRECORD*) malloc(sizeof (struct FIXUPP_16_FIXUP_SUBRECORD));
    // Frame datum should hold the index of the segment we are referring to.
    subrecord->frame_datum = ref_extern;
  
    subrecord->mode = fixup_mode;
    subrecord->location = location_type;

    subrecord->data_record_offset = offset;

    // Just a default for now, I don't know enough about it to do it properly.
    subrecord->fix_data = 0x56;

    return subrecord;
}

struct MODEND_16* BuildMODEND16(struct MagicOMFHandle* handle)
{
    // Very limited functionality for MODEND at the moment.
    struct MODEND_16* modend_16 = (struct MODEND_16*) malloc(sizeof (struct MODEND_16));
    modend_16->has_start_address = false;
    modend_16->is_main = true;
    return modend_16;
}

struct PUBDEF_16* BuildPUBDEF16(struct MagicOMFHandle* handle, const char* seg_name)
{
    struct PUBDEF_16* contents = (struct PUBDEF_16*) malloc(sizeof (struct PUBDEF_16));
    uint8 seg_index = MagicOMFGetSEGDEFIndex(handle, seg_name);
    // We don't support groups yet
    contents->bg_index = 0;
    contents->bs_index = seg_index;
    contents->segdef_16_record = MagicOMFGetSEGDEF16ByIndex(handle, seg_index);
    contents->iden = NULL;
    return contents;
}

struct PUBDEF_16_IDEN* BuildPUBDEF16_IDEN(const char* pub_def_name, uint16 offset, uint8 type_index)
{
    struct PUBDEF_16_IDEN* pubdef_16_iden = (struct PUBDEF_16_IDEN*) malloc(sizeof (struct PUBDEF_16_IDEN));
    pubdef_16_iden->str_len = strlen(pub_def_name);
    pubdef_16_iden->name_str = (char*) pub_def_name;
    pubdef_16_iden->p_offset = offset;
    pubdef_16_iden->type_index = type_index;
    pubdef_16_iden->next = NULL;
    return pubdef_16_iden;
}