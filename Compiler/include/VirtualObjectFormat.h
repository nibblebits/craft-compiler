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
 * File:   VirtualObjectFormat.h
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 15:41
 */

#ifndef VIRTUALOBJECTFORMAT_H
#define VIRTUALOBJECTFORMAT_H

#include <vector>
#include <memory>

#include "VirtualSegment.h"
#include "Stream.h"

class VirtualObjectFormat {
public:
    VirtualObjectFormat();
    virtual ~VirtualObjectFormat();
    
    std::shared_ptr<VirtualSegment> createSegment(std::string segment_name);
    std::shared_ptr<VirtualSegment> getSegment(std::string segment_name);   
    std::vector<std::shared_ptr<VirtualSegment>> getSegments();
    
    Stream* getObjectStream();
    
    virtual void finalize() = 0;
private:
    Stream object_stream;
    std::vector<std::shared_ptr<VirtualSegment>> segments;
};

#endif /* VIRTUALOBJECTFORMAT_H */

