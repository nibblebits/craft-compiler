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
 * File:   Compiler.cpp
 * Author: Daniel McCarthy
 *
 * Created on 18 June 2016, 18:53
 * 
 * Description: Holds all major entities to the compiler
 */

#include "Compiler.h"
#include "VDEFBranch.h"
#include "STRUCTBranch.h"
#include "STRUCTDEFBranch.h"
#include "ArrayIndexBranch.h"
#include "BODYBranch.h"

Compiler::Compiler()
{
    this->lexer = new Lexer(this);
    this->parser = new Parser(this);
    this->preprocessor = new Preprocessor(this);
    this->semanticValidator = new SemanticValidator(this);
    this->astAssistant = new ASTAssistant(this);
    this->treeImprover = new TreeImprover(this);
    this->codeGenerator = NULL;
    this->linker = NULL;
}

Compiler::~Compiler()
{

}

void Compiler::setArgument(std::string name, std::string value)
{
    this->arguments[name] = value;
}

void Compiler::setCodeGenerator(std::shared_ptr<CodeGenerator> codegen)
{
    if (this->codeGenerator != NULL)
        throw Exception("An existing code generator has already been set! Only one code generator may be set to the compiler");
    if (codegen == NULL)
        throw Exception("The code generator may not be NULL!");

    this->codeGenerator = codegen;
}

void Compiler::setLinker(std::shared_ptr<Linker> linker)
{
    this->linker = linker;
}

Lexer* Compiler::getLexer()
{
    return this->lexer;
}

Parser* Compiler::getParser()
{
    return this->parser;
}

Preprocessor* Compiler::getPreprocessor()
{
    return this->preprocessor;
}

SemanticValidator* Compiler::getSemanticValidator()
{
    return this->semanticValidator;
}

TreeImprover* Compiler::getTreeImprover()
{
    return this->treeImprover;
}

ASTAssistant* Compiler::getASTAssistant()
{
    return this->astAssistant;
}

std::shared_ptr<CodeGenerator> Compiler::getCodeGenerator()
{
    return this->codeGenerator;
}

std::shared_ptr<Linker> Compiler::getLinker()
{
    return this->linker;
}

std::string Compiler::getArgumentValue(std::string name)
{
    return this->arguments[name];
}

bool Compiler::hasArgument(std::string name)
{
    return this->arguments.find(name) != this->arguments.end();
}

std::shared_ptr<VDEFBranch> Compiler::getVariableFromStructure(std::shared_ptr<STRUCTBranch> structure, std::string var_name)
{
    for (std::shared_ptr<Branch> branch : structure->getStructBodyBranch()->getChildren())
    {
        if (branch->getType() == "V_DEF" ||
                branch->getType() == "STRUCT_DEF")
        {
            std::shared_ptr<VDEFBranch> v_def_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
            if (v_def_branch->getNameBranch()->getValue() == var_name)
            {
                return v_def_branch;
            }
        }
    }

    return NULL;
}

int Compiler::getSizeOfVarDef(std::shared_ptr<VDEFBranch> vdef_branch)
{
    int size = getDataTypeSizeFromVarDef(vdef_branch);
    std::shared_ptr<VarIdentifierBranch> var_iden_branch = vdef_branch->getVariableIdentifierBranch();
    if (var_iden_branch->hasRootArrayIndexBranch())
    {
        size *= getSumOfArrayIndexes(var_iden_branch->getRootArrayIndexBranch());
    }

    return size;
}

bool Compiler::isPrimitiveDataType(std::string type)
{
    // Should be changed to access an array.
    return (
            type == "void" ||
            type == "bit" ||
            type == "nibble" ||
            type == "uint8" ||
            type == "uint16" ||
            type == "uint32" ||
            type == "uint64" ||
            type == "int8" ||
            type == "int16" ||
            type == "int32" ||
            type == "int64"
            );
}

int Compiler::getPrimitiveDataTypeSize(std::string type)
{
    /* Note for now data types such as bit and nibble will consume 1 byte, this will hopefully be changed in the future to work at the bit level*/
    if (type == "bit" || type == "nibble" || type == "uint8" || type == "int8")
    {
        return 1;
    }
    else if (type == "uint16" || type == "int16")
    {
        return 2;
    }
    else if (type == "uint32" || type == "int32")
    {
        return 4;
    }
    else if (type == "uint64" || type == "int64")
    {
        return 8;
    }
    else
    {
        throw Exception("int Compiler::getDataTypeSize(std::string type): Invalid primative date type: " + type);
    }
}

int Compiler::getDataTypeSizeFromVarDef(std::shared_ptr<VDEFBranch> branch, bool no_pointer)
{
    std::string type = branch->getType();
    int size = 0;

    // Sometimes people may want to get the data type size as if the branch was not a pointer.
    if (!no_pointer)
    {
        if (branch->isPointer())
        {
            return this->codeGenerator->getPointerSize();
        }
    }
    if (type == "STRUCT_DEF")
    {
        std::shared_ptr<Tree> tree = this->parser->getTree();
        std::shared_ptr<STRUCTDEFBranch> struct_def_branch = std::dynamic_pointer_cast<STRUCTDEFBranch>(branch);
        std::shared_ptr<Branch> struct_def_data_type = struct_def_branch->getDataTypeBranch();
        std::shared_ptr<STRUCTBranch> struct_branch =
                std::dynamic_pointer_cast<STRUCTBranch>(tree->getGlobalStructureByName(struct_def_data_type->getValue()));

        size = getSizeOfStructure(struct_branch);
    }
    else if (type == "V_DEF")
    {
        std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(branch);
        size = getPrimitiveDataTypeSize(vdef_branch->getDataTypeBranch()->getValue());
    }

    return size;
}

int Compiler::getSizeOfStructure(std::shared_ptr<STRUCTBranch> structure)
{
    int t_size = 0;
    for (std::shared_ptr<Branch> struct_branch_child : structure->getStructBodyBranch()->getChildren())
    {
        std::shared_ptr<VDEFBranch> vdef_child_branch = std::dynamic_pointer_cast<VDEFBranch>(struct_branch_child);
        std::shared_ptr<VarIdentifierBranch> var_iden_child_branch = vdef_child_branch->getVariableIdentifierBranch();
        int size = getDataTypeSizeFromVarDef(std::dynamic_pointer_cast<VDEFBranch>(vdef_child_branch));
        // Check to see if we have an array going on here
        if (var_iden_child_branch->hasRootArrayIndexBranch())
        {
            // We need to multiply all of the array indexes together so we know how many elements we have
            std::shared_ptr<ArrayIndexBranch> array_index_branch = std::dynamic_pointer_cast<ArrayIndexBranch>(var_iden_child_branch->getRootArrayIndexBranch());
            int total_array_indexes = getSumOfArrayIndexes(array_index_branch);
            // Calculate the array size
            size = total_array_indexes * size;
        }

        t_size += size;
    }

    return t_size;
}

int Compiler::getSumOfArrayIndexes(std::shared_ptr<ArrayIndexBranch> root_array_index_branch)
{
    std::shared_ptr<ArrayIndexBranch> array_index_branch = root_array_index_branch;
    int total_array_indexes = stoi(array_index_branch->getValueBranch()->getValue());
    while (array_index_branch->hasNextArrayIndexBranch())
    {
        array_index_branch = std::dynamic_pointer_cast<ArrayIndexBranch>(array_index_branch->getNextArrayIndexBranch());
        total_array_indexes *= stoi(array_index_branch->getValueBranch()->getValue());
    }

    return total_array_indexes;
}

std::string Compiler::getTypeFromNumber(int number)
{
    if (number >= 0)
    {
        if (number <= 0xff)
        {
            return "uint8";
        }
        else if (number <= 0xffff)
        {
            return "uint16";
        }
        else if (number <= 0xffffffff)
        {
            return "uint32";
        }
    }
    else
    {
        if (number >= -0xff)
        {
            return "int8";
        }
        else if (number >= -0xffff)
        {
            return "int16";
        }
        else if (number >= -0xffffffff)
        {
            return "int32";
        }
    }
}

bool Compiler::canCast(std::string type1, std::string type2)
{
    if (type1 == "uint8" || type1 == "int8")
    {
        if (type2 == "uint8" || type2 == "int8" || type2 == "uint16" || type2 == "int16" || type2 == "uint32" || type2 == "int32")
        {
            return true;
        }
    }
    else if (type1 == "uint16" || type1 == "int16")
    {
        if (type2 == "uint32" || type2 == "int32")
        {
            return true;
        }
    }

    return false;
}

long Compiler::evaluate(long n1, long n2, std::string op)
{
    if (op == "+")
        return n1 + n2;
    else if (op == "-")
        return n1 - n2;
    else if (op == "*")
        return n1 * n2;
    else if (op == "/")
        return n1 / n2;
    else if (op == "%")
        return n1 % n2;
    else if (op == "^")
        return n1 ^ n2;
    else if (op == "|")
        return n1 | n2;
    else if (op == "&")
        return n1 & n2;
    else if (op == ">>")
        return n1 >> n2;
    else if (op == "<<")
        return n1 << n2;


    throw Exception("long Compiler::evaluate(long n1, long n2, std::string op): do not know how to evaluate for operator: \"" + op + "\"");
}

bool Compiler::isCompareOperator(std::string value)
{
    if (
            value == "==" ||
            value == "!=" ||
            value == "<=" ||
            value == ">=" ||
            value == "<" ||
            value == ">")
        return true;

    return false;
}

bool Compiler::isLogicalOperator(std::string value)
{
    if (
            value == "&&" ||
            value == "||")
        return true;

    return false;
}