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
 * File:   translate.h
 * Author: Daniel McCarthy
 *
 * Created on 06 December 2016, 12:37
 */

#ifndef TRANSLATE_H
#define TRANSLATE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "error.h"
#include "record.h"
    
struct RECORD* StartRecord();
void EndRecord(struct RECORD* record, struct MagicOMFHandle* handle);
void TranslatorSkipRecord(struct MagicOMFHandle* handle);
void TranslatorReadTHEADR(struct MagicOMFHandle* handle);
void TranslatorReadLHEADR(struct MagicOMFHandle* handle);
void TranslatorReadCOMENT_Translator(struct RECORD* record, struct MagicOMFHandle* handle);
void TranslatorReadCOMENT_LinkPassSeparator(struct RECORD* record, struct MagicOMFHandle* handle);
void TranslatorReadCOMENT(struct MagicOMFHandle* handle);
void TranslatorReadLNAMES(struct MagicOMFHandle* handle);
void TranslatorReadSEGDEF16(struct MagicOMFHandle* handle);
void TranslatorReadPUBDEF16(struct MagicOMFHandle* handle);
void TranslatorReadLEDATA16(struct MagicOMFHandle* handle);
void TranslatorReadFIXUPP16(struct MagicOMFHandle* handle);
void TranslatorReadFIXUPP16_FIXUP_SUBRECORD(uint16 locat, struct FIXUP_16_SUBRECORD_DESCRIPTOR* subrecord_descriptor, struct MagicOMFHandle* handle);
void TranslatorReadMODEND16(struct MagicOMFHandle* handle);
void TranslatorReadEXTDEF(struct MagicOMFHandle* handle);
void TranslatorFinalize(struct MagicOMFHandle* handle);
void TranslatorFinalize_FIXUPP_16(struct RECORD* record, struct MagicOMFHandle* handle);
#ifdef __cplusplus
}
#endif

#endif /* TRANSLATE_H */

