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
 * File:   DataTypeBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 01 March 2017, 03:38
 * 
 * Description: 
 */

#include "DataTypeBranch.h"

DataTypeBranch::DataTypeBranch(Compiler* compiler) : CustomBranch(compiler, "DATA_TYPE_BRANCH", "")
{
    this->custom_data_type_size = -1;
    this->is_pointer = false;
}

DataTypeBranch::~DataTypeBranch()
{

}

void DataTypeBranch::setDataType(std::string data_type)
{
    this->data_type = data_type;
}

std::string DataTypeBranch::getDataType()
{
    return this->data_type;
}

/**
 * Returns a formatted version of the data type.
 * 
 * For example: The data type branch has a type of "uint8" and has a pointer depth of 2.
 * This method would return "uint8**"
 * 
 * @return A formatted data type
 */
std::string DataTypeBranch::getDataTypeFormatted()
{
    std::string formatted_str = getDataType();

    for (int i = 0; i < getPointerDepth(); i++)
    {
        formatted_str += "*";
    }


    return formatted_str;
}

void DataTypeBranch::setPointer(bool is_pointer, int depth)
{
    this->is_pointer = is_pointer;
    this->pointer_depth = depth;
}

void DataTypeBranch::setCustomDataTypeSize(int size)
{
    this->custom_data_type_size = size;
}

bool DataTypeBranch::hasCustomDataTypeSize()
{
    return this->custom_data_type_size != -1;
}

int DataTypeBranch::getDataTypeSize(bool no_pointer_if_one_pointer)
{
    if (hasCustomDataTypeSize())
        return this->custom_data_type_size;

    // Sometimes people may want to get the data type size as if the branch was not a pointer.
    if (isPointer() && (!no_pointer_if_one_pointer || getPointerDepth() > 1))
    {
        return getCompiler()->getCodeGenerator()->getPointerSize();
    }

    // Is this a primitive type?
    if (getCompiler()->isPrimitiveDataType(getDataType()))
    {
        // Yes it is so return the primitive data type size
        return getCompiler()->getPrimitiveDataTypeSize(getDataType());
    }
    else
    {
        // This isn't a primitive type so it must be linking to a structure
        return getCompiler()->getSizeOfStructure(getCompiler()->getParser()->getTree()->getGlobalStructureByName(getDataType()));
    }

}

bool DataTypeBranch::isPointer()
{
    return this->is_pointer;
}

int DataTypeBranch::getPointerDepth()
{
    return this->pointer_depth;
}

bool DataTypeBranch::isSigned()
{
    std::string data_type_value = getDataType();
    return (
            data_type_value == "int8" ||
            data_type_value == "int16" ||
            data_type_value == "int32" ||
            data_type_value == "int64"
            );
}

bool DataTypeBranch::isPrimitive()
{
    std::string data_type = getDataType();
    return this->getCompiler()->isPrimitiveDataType(data_type);
}

void DataTypeBranch::imp_clone(std::shared_ptr<Branch> cloned_branch)
{
    std::shared_ptr<DataTypeBranch> cloned_data_type_branch = std::dynamic_pointer_cast<DataTypeBranch>(cloned_branch);
    if (cloned_data_type_branch->hasCustomDataTypeSize())
    {
        cloned_data_type_branch->setCustomDataTypeSize(this->custom_data_type_size);
    }
    cloned_data_type_branch->setDataType(getDataType());
    cloned_data_type_branch->setPointer(isPointer(), getPointerDepth());
}

std::shared_ptr<Branch> DataTypeBranch::create_clone()
{
    return std::shared_ptr<Branch>(new DataTypeBranch(getCompiler()));
}