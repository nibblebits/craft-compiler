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
#include <deque>
class Stream {
public:
    Stream();
    virtual ~Stream();
    void write8(uint8_t c);
    void write16(uint16_t s);
    void write32(uint32_t i);
    uint8_t read8();
    uint16_t read16();
    uint32_t read32();
private:
    std::deque<uint8_t> stack;
};

#endif /* STREAM_H */

