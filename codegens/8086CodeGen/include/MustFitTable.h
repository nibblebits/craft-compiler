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
 * File:   MustFitTable.h
 * Author: Daniel McCarthy
 *
 * Created on 26 January 2017, 02:50
 */

#ifndef MUSTFITTABLE_H
#define MUSTFITTABLE_H

#include <vector>
#include <memory>
#include <string>

class InstructionBranch;
class OperandBranch;

enum
{
    MUST_FIT_8_BIT_SIGNED,
    MUST_FIT_16_BIT_SIGNED
};

typedef int MUST_FIT_TYPE;
struct MUST_FIT
{
    MUST_FIT_TYPE must_fit;
    std::shared_ptr<InstructionBranch> ins_branch;
    std::shared_ptr<OperandBranch> operand_branch;
};

class MustFitTable {
public:
    MustFitTable(std::string label_name);
    MustFitTable();
    virtual ~MustFitTable();
    
    void setLabelName(std::string label_name);
    void addMustFit(MUST_FIT_TYPE must_fit, std::shared_ptr<InstructionBranch> ins_branch, std::shared_ptr<OperandBranch> op_branch);
    std::vector<struct MUST_FIT> getMustFits();
    bool hasMustFits();
    std::string getLabelName();
private:
    std::string label_name;
    std::vector<struct MUST_FIT> must_fits;
};

#endif /* MUSTFITTABLE_H */

