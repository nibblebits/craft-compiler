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
 * File:   generator.h
 * Author: dansb
 *
 * Created on 12 January 2017, 03:24
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "mdef.h"
#include "types.h"
#include "error.h"
#include "record.h"
#include "IO.h"

void GeneratorWriteRecordHeader(char** ptr, struct RECORD* record);
void GeneratorWriteTHEADR(char** ptr, struct RECORD* record);
void GeneratorWriteCOMENT(char** ptr, struct RECORD* record);
void GeneratorWriteLNAMES(char** ptr, struct RECORD* record);
void GeneratorWriteEXTDEF(char** ptr, struct RECORD* record);
void GeneratorWriteSEGDEF16(char** ptr, struct RECORD* record);
void GeneratorWriteLEDATA16(char** ptr, struct RECORD* record);
void GeneratorWriteFIXUPP16(char** ptr, struct RECORD* record);
void GeneratorWritePUBDEF16(char** ptr, struct RECORD* record);
void GeneratorWriteMODEND16(char** ptr, struct RECORD* record);
#ifdef __cplusplus
}
#endif

#endif /* GENERATOR_H */

