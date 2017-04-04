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

/* 
 * File:   mdef.h
 * Author: Daniel McCarthy
 *
 * Created on 06 December 2016, 15:13
 */

#ifndef MDEF_H
#define MDEF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "record.h"
enum
{
    THEADR_ID = 0x80,
    LHEADR_ID = 0x82,
    COMENT_ID = 0x88,
    LNAMES_ID = 0x96,
    SEGDEF_16_ID = 0x98,
    PUBDEF_16_ID = 0x90,
    LEDATA_16_ID = 0xa0,
    FIXUPP_16_ID = 0x9c,
    MODEND_16_ID = 0x8a,
    EXTDEF_ID = 0x8c
};

enum
{
    FIXUPP_THREAD_SUBRECORD,
    FIXUPP_FIXUP_SUBRECORD
};

enum
{
    FIXUPP_FIXUP_SET_F = 0x80,
    FIXUPP_FIXUP_SET_T = 0x08,
    FIXUPP_FIXUP_SET_P = 0x04
};
enum
{
    FIXUPP_MODE_SELF_RELATIVE_FIXUP,
    FIXUPP_MODE_SEGMENT_RELATIVE_FIXUP
};

enum
{
    FIXUPP_TARGET_TYPE_SEGIDX,
    FIXUPP_TARGET_TYPE_GRPIDX,
    FIXUPP_TARGET_TYPE_EXTIDX,
};

enum
{
    FIXUPP_LOCATION_LOW_ORDER_BYTE_8_BIT_DISPLACEMENT,
    FIXUPP_LOCATION_16_BIT_OFFSET,
    FIXUPP_LOCATION_16_BIT_BASE,
    FIXUPP_LOCATION_32_BIT_LONG_POINTER,
    FIXUPP_LOCATION_HIGH_ORDER_BYTE,
    FIXUPP_LOCATION_16_BIT_LOADER_RESOLVED_OFFSET
};

enum
{
    SEG_ATTR_ALIGNMENT_ABS_SEG,
    SEG_ATTR_ALIGNMENT_RELOC_BYTE_ALIGNED,
    SEG_ATTR_ALIGNMENT_RELOC_WORD_ALIGNED,
    SEG_ATTR_ALIGNMENT_PARAGRAPH_WORD_ALIGNED,
    SEG_ATTR_ALIGNMENT_RELOC_PAGE_BOUNDARY_ALIGNED,
    SEG_ATTR_ALIGNMENT_DOUBLE_WORD_ALIGNED,
    SEG_ATTR_ALIGNMENT_NOT_SUPPORTED,
    SEG_ATTR_ALIGNMENT_NOT_DEFINED
};

enum
{
    SEG_ATTR_COMBINATION_PRIVATE,
    SEG_ATTR_COMBINATION_RESERVED_1,
    SEG_ATTR_COMBINATION_PUBLIC_2,
    SEG_ATTR_COMBINATION_RESERVED_3,
    SEG_ATTR_COMBINATION_PUBLIC_4,
    SEG_ATTR_COMBINATION_STACK,
    SEG_ATTR_COMBINATION_COMMON,
    SEG_ATTR_COMBINATION_PUBLIC_7,
};

enum
{
    SEG_ATTR_P_USE16,
    SEG_ATTR_P_USE32
};

enum
{
    COMENT_CLASS_TRANSLATOR = 0,
    COMENT_CLASS_INTEL_COPYRIGHT = 1,
    COMENT_CLASS_INTEL_RESERVED_RANGE_START = 2,
    COMENT_CLASS_INTEL_RESERVED_RANGE_END = 0x9b,
    COMENT_CLASS_LIBRARY_SPECIFIER_OBSOLETE = 0x81,
    COMENT_CLASS_MSDOS_VERSION_OBSOLETE = 0x9c,
    COMENT_CLASS_MEMORY_MODEL = 0x9d,
    COMENT_CLASS_DOSSEG = 0x9e,
    COMENT_CLASS_DEFAULT_LIB_SEARCH_NAME = 0x9f,
    COMENT_CLASS_OMF_EXTS = 0xa0,
    COMENT_CLASS_NEW_OMF_EXT = 0xa1,
    COMENT_CLASS_LINK_PASS_SEPARATOR = 0xa2,
    COMENT_CLASS_LISMOD = 0xa3,
    COMENT_CLASS_EXESTR = 0xa4,
    COMENT_CLASS_INCERR = 0xa6,
    COMENT_CLASS_NOPAD = 0xa7,
    COMENT_CLASS_WKEXT = 0xa8,
    COMENT_CLASS_LZEXT = 0xa9,
    COMENT_CLASS_COMENT = 0xda,
    COMENT_CLASS_COMPILER = 0xdb,
    COMENT_CLASS_DATE = 0xdc,
    COMENT_CLASS_TIMESTAMP = 0xdd,
    COMENT_CLASS_USER = 0xdf,
    COMENT_CLASS_DEPENDENCY_FILE = 0xe9,
    COMENT_CLASS_COMMAND_LINE = 0xff,


};

enum
{
    COMENT_NO_LIST = 0x40,
    COMENT_NO_PURGE = 0x80
};

enum
{
    EXTDEF_TYPE_INDEX_NO_TYPDEF
};

typedef unsigned int FIXUP_MODE;

struct MagicOMFHandle
{
    char* buf;
    int buf_size;

    char* next;
    struct RECORD* root;
    struct RECORD* last;

    bool skip_unimplemented_records;
    MAGIC_OMF_ERROR_CODE last_error_code;
    bool has_error;
    int record_type;
    
    struct LEDATA_16* last_ledata;
};


#ifdef __cplusplus
}
#endif

#endif /* MDEF_H */

