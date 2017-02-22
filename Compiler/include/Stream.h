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
#include <memory>
#include <vector>
#include <algorithm>
#include <map>
#include "Exception.h"
#include "def.h"

class EXPORT Stream : public std::enable_shared_from_this<Stream>
{
public:
    Stream();
    virtual ~Stream();
    void loadFromFile(std::string filename);
    void loadFrom_ifstream(std::ifstream* stream);
    void setPosition(int position);
    void setOverwriteMode(bool overwrite_mode);

    void write8(uint8_t c, int pos = -1, bool ignore_joined_parents=false);
    void write16(uint16_t s);
    void write32(uint32_t i);
    void writeStr(std::string str, bool write_null_terminator = true, size_t fill_to = -1);
    void writeStr(const char* str, bool write_null_terminator = true, size_t fill_to = -1);
    void writeStream(Stream* stream, int offset = -1, int total = -1);
    void writeStream(std::shared_ptr<Stream> stream, int offset = -1, int total = -1);
    void joinStream(std::shared_ptr<Stream> stream);

    void overwrite8(int pos, uint8_t c);
    void overwrite16(int pos, uint16_t s);
    void overwrite32(int pos, uint32_t i);

    uint8_t peek8(int pos);
    uint16_t peek16(int pos);
    uint32_t peek32(int pos);

    uint8_t read8();
    uint16_t read16();
    uint32_t read32();
    std::string readStr();
    
    std::vector<std::shared_ptr<Stream>> chunkSplit(int chunk_size);
    size_t getSize();
    std::shared_ptr<Stream> getJoinedStreamForPosition(int pos);
    int getJoinedStreamPosition(std::shared_ptr<Stream> stream);
    [[deprecated("There is a bug with isEmpty avoid usage until a fix is made")]]
    bool isEmpty();
    bool hasInput();
    bool hasJoinedChild(std::shared_ptr<Stream> stream);
    bool hasJoinedParent(std::shared_ptr<Stream> stream);
    bool isJointWith(std::shared_ptr<Stream> stream);
    bool isOverwriteModeEnabled();

    void empty();
    int getPosition();

    char* getBuf();
    char* toNewBuf();

protected:
    void newJointParent(std::shared_ptr<Stream> stream);
    void updateDataForJoinedParents(uint8_t c, int pos_rel_to_us);
private:
    std::vector<uint8_t> vector;
    std::map<std::shared_ptr<Stream>, int> joined_streams;
    std::vector<std::shared_ptr<Stream>> parent_joined_streams;
    bool overwrite_mode;
    int pos;
    int joint_pos;
};

#endif /* STREAM_H */

