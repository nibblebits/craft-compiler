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
 * File:   OMFObjectFormat.h
 * Author: Daniel McCarthy
 *
 * Created on 11 January 2017, 22:10
 */

#ifndef OMFOBJECTFORMAT_H
#define OMFOBJECTFORMAT_H
#include "MagicOMF.h"
#include "VirtualObjectFormat.h"

class EXPORT OMFObjectFormat : public VirtualObjectFormat {
public:
    OMFObjectFormat(Compiler* compiler);
    virtual ~OMFObjectFormat();

    virtual std::shared_ptr<VirtualSegment> new_segment(std::string segment_name, uint32_t origin);
    virtual void read(std::shared_ptr<Stream> input_stream);
    virtual void finalize();

private:
    LOCATION_TYPE get_location_type_from_fixup_standard(std::shared_ptr<FIXUP_STANDARD> fixup_standard);
    void handle_segment_fixup(struct RECORD* record, std::shared_ptr<SEGMENT_FIXUP> seg_fixup);
    void handle_extern_fixup(struct RECORD* record, std::shared_ptr<EXTERN_FIXUP> extern_fixup);
};

#endif /* OMFOBJECTFORMAT_H */

