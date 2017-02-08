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
 * File:   BinLinker.h
 * Author: Daniel McCarthy
 *
 * Created on 06 February 2017, 19:09
 */

#ifndef BINLINKER_H
#define BINLINKER_H

#include "Linker.h"
#include "VirtualObjectFormat.h"

class BinLinker : public Linker {
public:
    BinLinker(Compiler* compiler);
    virtual ~BinLinker();
protected:
    int countStreamSizesStopAtSegment(std::shared_ptr<VirtualObjectFormat> obj, std::shared_ptr<VirtualSegment> segment_to_stop);
    void WriteSegment(Stream* executable_stream, std::shared_ptr<VirtualSegment> segment);
    virtual void resolve(std::shared_ptr<VirtualObjectFormat> final_obj);
    virtual void build(Stream* executable_stream, std::shared_ptr<VirtualObjectFormat> final_obj);
private:

};

#endif /* BINLINKER_H */

