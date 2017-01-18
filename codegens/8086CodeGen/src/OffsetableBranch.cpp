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
 * File:   OffsetableBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 01 January 2017, 21:32
 * 
 * Description: 
 */

#include "OffsetableBranch.h"

OffsetableBranch::OffsetableBranch(Compiler* compiler, std::shared_ptr<SegmentBranch> segment_branch, std::string type, std::string value) : ChildOfSegment(compiler, segment_branch, type, value)
{
}

OffsetableBranch::~OffsetableBranch()
{
}

void OffsetableBranch::setOffset(int offset)
{
    this->offset = offset;
}

int OffsetableBranch::getOffset()
{
    return this->offset;
}

void OffsetableBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    ChildOfSegment::imp_clone(cloned_branch);
    std::shared_ptr<OffsetableBranch> cloned_offsetable_branch = std::dynamic_pointer_cast<OffsetableBranch>(cloned_branch);
    cloned_offsetable_branch->setOffset(getOffset());
}

std::shared_ptr<Branch> OffsetableBranch::create_clone()
{
    return std::shared_ptr<OffsetableBranch>(new OffsetableBranch(getCompiler(), getSegmentBranch(), getType(), getValue()));
}