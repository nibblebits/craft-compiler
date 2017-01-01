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
 * File:   SegmentBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 21 December 2016, 15:34
 * 
 * Description: 
 */

#include "SegmentBranch.h"

SegmentBranch::SegmentBranch(Compiler* compiler) : CustomBranch(compiler, "SEGMENT", "")
{
}

SegmentBranch::~SegmentBranch()
{
}

void SegmentBranch::setSegmentNameBranch(std::shared_ptr<Branch> label_name_branch)
{
    CustomBranch::registerBranch("segment_name_branch", label_name_branch);
}

std::shared_ptr<Branch> SegmentBranch::getSegmentNameBranch()
{
    return CustomBranch::getRegisteredBranchByName("segment_name_branch");
}

void SegmentBranch::setContentsBranch(std::shared_ptr<Branch> contents_branch)
{
    CustomBranch::registerBranch("contents_branch", contents_branch);
}

std::shared_ptr<Branch> SegmentBranch::getContentsBranch()
{
    return CustomBranch::getRegisteredBranchByName("contents_branch");
}

void SegmentBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<SegmentBranch> label_branch = std::dynamic_pointer_cast<SegmentBranch>(cloned_branch);
    label_branch->setSegmentNameBranch(getSegmentNameBranch()->clone());
    
}

std::shared_ptr<Branch> SegmentBranch::create_clone()
{
    return std::shared_ptr<Branch>(new SegmentBranch(getCompiler()));
}