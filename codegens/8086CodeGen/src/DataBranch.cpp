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
 * File:   DataBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 15 January 2017, 15:49
 * 
 * Description: 
 */

#include "DataBranch.h"

DataBranch::DataBranch(Compiler* compiler) : OffsetableBranch(compiler, "DATA", "")
{
    this->type = -1;
}

DataBranch::~DataBranch()
{
}

void DataBranch::setDataBranchType(DATA_BRANCH_TYPE type)
{
    this->type = type;
}

DATA_BRANCH_TYPE DataBranch::getDataBranchType()
{
    return this->type;
}

void DataBranch::setData(std::shared_ptr<Branch> branch)
{
    CustomBranch::registerBranch("data_branch", branch);
}

std::shared_ptr<Branch> DataBranch::getData()
{
    return CustomBranch::getRegisteredBranchByName("data_branch");
}

void DataBranch::setNextDataBranch(std::shared_ptr<DataBranch> data_branch)
{
    CustomBranch::registerBranch("next_data_branch", data_branch);
}

std::shared_ptr<DataBranch> DataBranch::getNextDataBranch()
{
    return std::dynamic_pointer_cast<DataBranch>(CustomBranch::getRegisteredBranchByName("next_data_branch"));
}

bool DataBranch::hasNextDataBranch()
{
    return CustomBranch::isBranchRegistered("next_data_branch");
}

void DataBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<DataBranch> data_branch_cloned = std::dynamic_pointer_cast<DataBranch>(cloned_branch);
    data_branch_cloned->setDataBranchType(data_branch_cloned->getDataBranchType());
    data_branch_cloned->setData(getData()->clone());
    data_branch_cloned->setNextDataBranch(std::dynamic_pointer_cast<DataBranch>(getNextDataBranch()->clone()));
}

std::shared_ptr<Branch> DataBranch::create_clone()
{
    return std::shared_ptr<Branch>(new DataBranch(getCompiler()));
}
