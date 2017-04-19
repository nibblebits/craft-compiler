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
 * File:   BlankVirtualObjectFormat.h
 * Author: Daniel McCarthy
 *
 * Created on 18 April 2017, 01:38
 * 
 * Description: Simple implemented VirtualObjectFormat that cannot read or write.
 * Can be used as a container for linking with libraries with Craft compiler.
 */

#ifndef BLANKVIRTUALOBJECTFORMAT_H
#define BLANKVIRTUALOBJECTFORMAT_H

#include "VirtualObjectFormat.h"

class EXPORT BlankVirtualObjectFormat : public VirtualObjectFormat
{
public:
    BlankVirtualObjectFormat(Compiler* compiler);
    virtual ~BlankVirtualObjectFormat();

    virtual void read(std::shared_ptr<Stream> input_stream);
    virtual void finalize();
    virtual std::shared_ptr<VirtualSegment> new_segment(std::string segment_name, uint32_t origin);
private:

};

#endif /* BLANKVIRTUALOBJECTFORMAT_H */

