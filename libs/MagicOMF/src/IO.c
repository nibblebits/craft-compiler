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
 * File:   IO.c
 * Author: Daniel McCarthy
 *
 * Created on 06 December 2016, 12:16
 */

#include <stdlib.h>
#include <stdio.h>
#include "IO.h"

void ReadAndIgnoreBytes(char** ptr, char* end)
{
    while (*ptr != end)
    {
        ReadUnsignedByte(ptr);
    }
}

uint8 ReadUnsignedByteNoNext(char* ptr)
{
    uint8 v = *ptr;
    return v;
}

uint16 ReadUnsignedWordNoNext(char* ptr)
{
    uint8 l = *ptr;
    uint8 h = *(ptr + 1);
    uint16 word = (h << 8) | (l);
    return word;
}

char* ReadStringAddTerminator(char** ptr, uint8 size)
{
    char* buf = (char*) malloc(size + 1);
    for (uint8 i = 0; i < size; i++)
    {
        buf[i] = **ptr;
        *ptr += 1;
    }
    buf[size] = 0;
    return buf;
}

char* ReadStringUntilEndAddTerminator(char** ptr, char* end)
{
    int size = (int) (end - *ptr);
    return ReadStringAddTerminator(ptr, size);
}

char* ReadDataUntilEnd(char** ptr, char* end)
{
    int size = (int) (end - *ptr);
    char* buf = (char*) malloc(size);
    for (int i = 0; i < size; i++)
    {
        buf[i] = **ptr;
        *ptr += 1;
    }

    return buf;
}

uint8 ReadUnsignedByte(char** ptr)
{
    uint8 b = ReadUnsignedByteNoNext(*ptr);
    *ptr += 1;
    return b;
}

uint16 ReadUnsignedWord(char** ptr)
{
    uint16 word = ReadUnsignedWordNoNext(*ptr);
    *ptr += 2;
    return word;
}

uint32 ReadUnsignedInt(char** ptr)
{
    uint32 swapped = ((**ptr >> 24)&0xff) | // move byte 3 to byte 0
            ((**ptr << 8)&0xff0000) | // move byte 1 to byte 2
            ((**ptr >> 8)&0xff00) | // move byte 2 to byte 1
            ((**ptr << 24)&0xff000000); // byte 0 to byte 3

    return swapped;
}

void WritingToPointer(char** writing_ptr)
{
    ptr = writing_ptr;
}

void WriteUnsignedByte(uint8 value)
{
    **ptr = value;
    *ptr += 1;
}

void WriteUnsignedWord(uint16 value)
{
    **ptr = value;
    *ptr += 1;
    **ptr = value >> 8;
    *ptr += 1;
}

void WriteUnsignedInt(uint16 value)
{
    // Not yet supported.
}

void WriteStringNoTerminator(char* string, uint16 size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        **ptr = string[i];
        *ptr += 1;
    }
}

void WriteData(char* data, uint16 size)
{
    for (uint16 i = 0; i < size; i++)
    {
        **ptr = data[i];
        *ptr += 1;
    }
}