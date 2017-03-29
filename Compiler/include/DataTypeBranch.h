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
 * File:   DataTypeBranch.h
 * Author: Daniel McCarthy
 *
 * Created on 01 March 2017, 03:38
 */

#ifndef DATATYPEBRANCH_H
#define DATATYPEBRANCH_H

#include "CustomBranch.h"

class EXPORT DataTypeBranch : public CustomBranch
{
public:
    DataTypeBranch(Compiler* compiler);
    virtual ~DataTypeBranch();

    void setDataType(std::string keyword);
    std::string getDataType();
    std::string getDataTypeFormatted();
    void setPointer(bool is_pointer, int depth = 0);
    void setCustomDataTypeSize(int size);
    bool hasCustomDataTypeSize();
    int getDataTypeSize(bool no_pointer = false);

    bool isPointer();
    int getPointerDepth();

    bool isSigned();
    bool isPrimitive();

    virtual void imp_clone(std::shared_ptr<Branch> cloned_branch);
    virtual std::shared_ptr<Branch> create_clone();

private:
    std::string data_type;
    int custom_data_type_size;
    bool is_pointer;
    int pointer_depth;
};

#endif /* DATATYPEBRANCH_H */

