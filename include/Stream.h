/*
    Goblin compiler v1.0 - The standard compiler for the Goblin language.
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
#include "Stack.h"
#include "Exception.h"

class Stream {
public:
    Stream();
    virtual ~Stream();
    void setPosition(size_t position);
    void write8(uint8_t c);
    void write16(uint16_t s);
    void write32(uint32_t i);
    void writeStr(std::string str, size_t fill_to = -1);
    void writeStr(const char* str, size_t fill_to = -1);
    uint8_t read8();
    uint16_t read16();
    uint32_t read32();
    std::string readStr();
    size_t getSize();
    bool isEmpty();
    void empty();
    int getPosition();
    void startLoggingOffset();
    void stopLoggingOffset();
    bool isLoggingOffset();
    int getLoggedOffset();
private:
    Stack<uint8_t> stack;
    int offset;
};

#endif /* STREAM_H */

