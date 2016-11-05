/*
    Goblin compiler v1.0 - The standard compiler for the Goblin language.
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
 * File:   VDEFBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 25 June 2016, 02:31
 * 
 * Description: The branch object for a "VDEF" branch.
 */

#include "VDEFBranch.h"
#include "ArrayBranch.h"
#include "PTRBranch.h"
#include "VarIdentifierBranch.h"

VDEFBranch::VDEFBranch(Compiler* compiler, std::string branch_name, std::string branch_value) : CustomBranch(compiler, branch_name, branch_value)
{

}

VDEFBranch::~VDEFBranch()
{
}

void VDEFBranch::setDataTypeBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("data_type_branch", branch);
}

void VDEFBranch::setVariableBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("var_branch", branch);
}

void VDEFBranch::setValueExpBranch(std::shared_ptr<Branch> branch)
{
    this->registerBranch("value_exp_branch", branch);
}

std::shared_ptr<Branch> VDEFBranch::getDataTypeBranch()
{
    return this->getRegisteredBranchByName("data_type_branch");
}

std::shared_ptr<Branch> VDEFBranch::getVariableBranch()
{
    return this->getRegisteredBranchByName("var_branch");
}

std::shared_ptr<Branch> VDEFBranch::getValueExpBranch()
{
    return this->getRegisteredBranchByName("value_exp_branch");
}

std::shared_ptr<Branch> VDEFBranch::getNameBranch()
{
    std::shared_ptr<Branch> identifier_branch = getVariableBranch();
    std::string type = identifier_branch->getType();
    if (type == "PTR")
    {
        std::shared_ptr<PTRBranch> ptr_branch = std::dynamic_pointer_cast<PTRBranch>(identifier_branch);
        std::shared_ptr<VarIdentifierBranch> var_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(ptr_branch->getVariableBranch());
        return var_branch->getVariableNameBranch();
    } else if(type == "VAR_IDENTIFIER")
    {
        std::shared_ptr<VarIdentifierBranch> var_branch = std::dynamic_pointer_cast<VarIdentifierBranch>(identifier_branch);
        return var_branch->getVariableNameBranch();
    }
    else
    {
        throw new Exception("VDEFBranch::getNameBranch(): could not locate the name branch");
    }
}

bool VDEFBranch::isPointer()
{
    std::shared_ptr<Branch> branch = getVariableBranch();
    return (branch->getType() == "PTR");
}

bool VDEFBranch::isSigned()
{
    std::shared_ptr<Branch> data_type_branch = getDataTypeBranch();
    std::string data_type_value = data_type_branch->getValue();
    return (
            data_type_value == "int8" ||
            data_type_value == "int16" ||
            data_type_value == "int32" ||
            data_type_value == "int64"
            );
}

int VDEFBranch::getDataTypeSize()
{
    if (isPointer())
    {
        throw Exception("VDEFBranch::getDataTypeSize(): the size of a pointer depends on the system architecture");
    }

    return this->getCompiler()->getDataTypeSize(getDataTypeBranch()->getValue());
}