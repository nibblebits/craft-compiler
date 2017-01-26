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
 * File:   LabelBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 18:34
 */

#ifndef LABELBRANCH_H
#define LABELBRANCH_H

#include "OffsetableBranch.h"

class MustFitTable;
class LabelBranch : public OffsetableBranch
{
public:
    LabelBranch(Compiler* compiler, std::shared_ptr<SegmentBranch> segment_branch);
    LabelBranch(Compiler* compiler, std::shared_ptr<SegmentBranch> segment_branch, std::shared_ptr<MustFitTable> must_fit_table);
    
    virtual ~LabelBranch();

    void setLabelNameBranch(std::shared_ptr<Branch> label_name_branch);
    std::shared_ptr<Branch> getLabelNameBranch();
    void setContentsBranch(std::shared_ptr<Branch> contents_branch);
    std::shared_ptr<Branch> getContentsBranch();

    std::shared_ptr<MustFitTable> getMustFitTable();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();
private:
    std::shared_ptr<MustFitTable> must_fit_table;
};

#endif /* LABELBRANCH_H */

