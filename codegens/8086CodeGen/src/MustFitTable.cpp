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
 * File:   OffsetFixupTable.cpp
 * Author: Daniel McCarthy
 *
 * Created on 26 January 2017, 02:50
 * 
 * Description: 
 */

#include "MustFitTable.h"
#include "InstructionBranch.h"
#include "OperandBranch.h"

MustFitTable::MustFitTable(std::string label_name)
{
    this->label_name = label_name;
}

MustFitTable::MustFitTable()
{
    this->label_name = "NOT_DEFINED";
}

MustFitTable::~MustFitTable()
{
}

void MustFitTable::setLabelName(std::string label_name)
{
    this->label_name = label_name;
}

void MustFitTable::addMustFit(MUST_FIT_TYPE must_fit, std::shared_ptr<InstructionBranch> ins_branch, std::shared_ptr<OperandBranch> op_branch)
{
    struct MUST_FIT must_fit_elem;
    must_fit_elem.must_fit = must_fit;
    must_fit_elem.ins_branch = ins_branch;
    must_fit_elem.operand_branch = op_branch;
    this->must_fits.push_back(must_fit_elem);
}

std::vector<struct MUST_FIT> MustFitTable::getMustFits()
{
    return this->must_fits;
}

bool MustFitTable::hasMustFits()
{
    return !this->must_fits.empty();
}

std::string MustFitTable::getLabelName()
{
    return this->label_name;
}
