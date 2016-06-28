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
 * File:   Stream.cpp
 * Author: Daniel McCarthy
 *
 * Created on 20 June 2016, 01:18
 * 
 * Description: A basic stream class
 * 
 * Note: This stream class may not support big endian machines at its current state.
 * 
 * All write operations push to the back of the stack and all read operations read from the front of the stack.
 */

#include "Stream.h"
Stream::Stream()
{
    setPosition(0);
    this->offset = -1;
}

Stream::~Stream()
{
}

void Stream::setPosition(size_t position)
{
    this->stack.setSP(position);
}

void Stream::write8(uint8_t c)
{
    this->stack.push(c);
    if (this->isLoggingOffset())
    {
        this->offset++;
    }
}

void Stream::write16(uint16_t s)
{
    uint8_t c1 = s & 0xff;
    uint8_t c2 = s >> 8;
    write8(c1);
    write8(c2);
}

void Stream::write32(uint32_t i)
{
    uint16_t s1 = i & 0xffff;
    uint16_t s2 = i >> 16;
    write16(s1);
    write16(s2);
}

uint8_t Stream::read8()
{
    uint8_t c = this->stack.pop_first();
    return c;
}

uint16_t Stream::read16()
{
    uint8_t c1 = read8();
    uint8_t c2 = read8();

    uint16_t result = (c2 << 8 | c1);
    return result;
}

uint32_t Stream::read32()
{
    uint16_t s1 = read16();
    uint16_t s2 = read16();

    uint32_t result = (s1 << 16 | s2);
    return result;
}

size_t Stream::getSize()
{
    return this->stack.size();
}

int Stream::getPosition()
{
    return this->stack.getSP();
}

/* Some helper methods for people using the stream class, it helps with offsetting */
void Stream::startLoggingOffset()
{
    this->offset = 0;
}
void Stream::stopLoggingOffset()
{
    this->offset = -1;
}

bool Stream::isLoggingOffset()
{
    return this->offset != -1;
}

int Stream::getLoggedOffset()
{
    if (!isLoggingOffset())
    {
        throw Exception("Stream::getLoggedOffset(): Offset logging has not been started");
    }
    
    return this->offset;
}