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
 * File:   Stream.h
 * Author: Daniel McCarthy
 *
 * Created on 20 June 2016, 01:18
 */

#ifndef STREAM_H
#define STREAM_H

#include <stdint.h>
#include <cstring>
#include <fstream>
#include <vector>
#include "Exception.h"
#include "def.h"

class EXPORT Stream
{
public:
    Stream();
    virtual ~Stream();
    void loadFromFile(std::string filename);
    void loadFrom_ifstream(std::ifstream* stream);
    void setPosition(size_t position);
    void write8(uint8_t c);
    void write16(uint16_t s);
    void write32(uint32_t i);
    void writeStr(std::string str, bool write_null_terminator = true, size_t fill_to = -1);
    void writeStr(const char* str, bool write_null_terminator = true, size_t fill_to = -1);

    uint8_t read8();
    uint16_t read16();
    uint32_t read32();
    std::string readStr();
    size_t getSize();
    bool isEmpty();
    bool hasInput();
    void empty();
    int getPosition();

    char* getBuf();
    char* toNewBuf();

private:
    std::vector<uint8_t> vector;
    int pos;
};

#endif /* STREAM_H */

