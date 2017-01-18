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
 * File:   LabelBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 18:34
 * 
 * Description: 
 */

#include "LabelBranch.h"

LabelBranch::LabelBranch(Compiler* compiler, std::shared_ptr<SegmentBranch> segment_branch) : OffsetableBranch(compiler, segment_branch, "LABEL", "")
{

}

LabelBranch::~LabelBranch()
{
}

void LabelBranch::setLabelNameBranch(std::shared_ptr<Branch> label_name_branch)
{
    CustomBranch::registerBranch("label_name_branch", label_name_branch);
}

std::shared_ptr<Branch> LabelBranch::getLabelNameBranch()
{
    return CustomBranch::getRegisteredBranchByName("label_name_branch");
}

void LabelBranch::setContentsBranch(std::shared_ptr<Branch> contents_branch)
{
    CustomBranch::registerBranch("contents_branch", contents_branch);
}

std::shared_ptr<Branch> LabelBranch::getContentsBranch()
{
    return CustomBranch::getRegisteredBranchByName("contents_branch");
}

void LabelBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<LabelBranch> label_branch = std::dynamic_pointer_cast<LabelBranch>(cloned_branch);
    label_branch->setLabelNameBranch(getLabelNameBranch()->clone());
}

std::shared_ptr<Branch> LabelBranch::create_clone()
{
    return std::shared_ptr<Branch>(new LabelBranch(getCompiler(), getSegmentBranch()));
}