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
 * File:   Preprocessor.cpp
 * Author: Daniel McCarthy
 *
 * Created on 08 January 2017, 01:58
 * 
 * Description: 
 */

#include <ctype.h>

#include "Preprocessor.h"
#include "Tree.h"
#include "branches.h"

Preprocessor::Preprocessor(Compiler* compiler) : CompilerEntity(compiler)
{

}

Preprocessor::~Preprocessor()
{
}

void Preprocessor::setTree(std::shared_ptr<Tree> tree)
{
    this->tree = tree;
}

void Preprocessor::process()
{
    std::shared_ptr<Branch> root = this->tree->root;
    // Lets get all the children that are macros and skip everything else
    for (std::shared_ptr<Branch> child : root->getChildren())
    {
        if (is_macro(child->getType()))
        {
            process_macro(child);
        }
    }
}

bool Preprocessor::is_macro(std::string macro_name)
{
    return (macro_name == "MACRO_IFDEF" || macro_name == "MACRO_DEFINE");
}

bool Preprocessor::is_definition_registered(std::string definition_name)
{
    return (this->definitions.find(definition_name) != this->definitions.end());
}

void Preprocessor::define_definition(std::string definition_name, std::string value)
{
    this->definitions[definition_name] = value;
}

std::string Preprocessor::get_definition_value(std::string definition_name)
{
    if (!is_definition_registered(definition_name))
    {
        // Will change this to use error routines like shown in parser, this place holder will work for now.
        throw Exception("std::string Preprocessor::get_definition_value(std::string definition_name): the definition does not exist");
    }

    return this->definitions[definition_name];
}

void Preprocessor::process_macro(std::shared_ptr<Branch> macro)
{
    if (macro->getType() == "MACRO_IFDEF")
    {
        std::shared_ptr<MacroIfDefBranch> macro_ifdef_branch = std::dynamic_pointer_cast<MacroIfDefBranch>(macro);
        process_macro_ifdef(macro_ifdef_branch);
    }
    else if (macro->getType() == "MACRO_DEFINE")
    {
        std::shared_ptr<MacroDefineBranch> macro_define_branch = std::dynamic_pointer_cast<MacroDefineBranch>(macro);
        process_macro_define(macro_define_branch);
    }
}

void Preprocessor::process_macro_ifdef(std::shared_ptr<MacroIfDefBranch> macro_ifdef_branch)
{
    std::shared_ptr<Branch> req_branch = macro_ifdef_branch->getRequirementBranch();
    std::shared_ptr<BODYBranch> body_branch = macro_ifdef_branch->getBodyBranch();

    if (is_definition_registered(req_branch->getValue()))
    {
        // The definition is registered so replace this macro with the body branch
        macro_ifdef_branch->replaceSelf(body_branch);
        body_branch->replaceWithChildren();
    }
    else
    {
        // All were false so lets delete ourself and all children associated with us
        macro_ifdef_branch->removeSelf();
    }
}

void Preprocessor::process_macro_define(std::shared_ptr<MacroDefineBranch> macro_define_branch)
{
    std::shared_ptr<Branch> def_name_branch = macro_define_branch->getDefinitionNameBranch();
    std::string value_str = "";
    if (macro_define_branch->hasDefinitionValueBranch())
    {
        // Ok this definition has a value so lets evaluate it
        std::shared_ptr<Branch> def_value_exp_branch = macro_define_branch->getDefinitionValueBranch();
        value_str = evaluate_expression(def_value_exp_branch);
    }

    // Define this definition
    define_definition(def_name_branch->getValue(), value_str);
    
    // Finally lets delete this define branch, we do not want the code generator to see this
    macro_define_branch->removeSelf();
}

std::string Preprocessor::evaluate_expression(std::shared_ptr<Branch> value_branch)
{
    std::string result = "";
    if (value_branch->getType() == "E")
    {
        std::string left_value, right_value, op;
        std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(value_branch);
        std::shared_ptr<Branch> left_branch = e_branch->getFirstChild();
        std::shared_ptr<Branch> right_branch = e_branch->getSecondChild();
        op = e_branch->getValue();

        if (left_branch->getType() == "E")
        {
            left_value = evaluate_expression(left_branch);
        }
        else
        {
            left_value = evaluate_expression_part(left_branch);
        }

        if (right_branch->getType() == "E")
        {
            right_value = evaluate_expression(right_branch);
        }
        else
        {
            right_value = evaluate_expression_part(right_branch);
        }

        /* At this point the left_value and right_value variables should contain strings to append or add together 
         * we need to figure out if we should add these strings together assuming they represent numeric values, or append them 
         * which will be the case if one or both strings are non-numeric.*/

        if (is_string_numeric_only(left_value) && is_string_numeric_only(right_value))
        {
            // Ok both are numeric we can evaluate them
            result = std::to_string(getCompiler()->evaluate(std::stoi(left_value), std::stoi(right_value), op));
        }
        else
        {
            // Either the left value or the right value is not completely numeric so we will just append them
            // While appending you must use the "+" operator
            if (op != "+")
            {
                // Will be replaced to use the error logger that the parser uses 
                throw Exception("std::string Preprocessor::evaluate_expression(std::shared_ptr<Branch> value_branch): either left value or right value is non numeric, "
                                "so we tried to append but was expecting operator \"+\" but operator \"" + op + "\" was provided.");
            }

            // Ok lets append them
            result.append(left_value);
            result.append(right_value);
        }
    }
    else
    {
        // Not an expression? no problem lets just evaluate one expression part
        result = evaluate_expression_part(value_branch);
    }

    return result;
}

std::string Preprocessor::evaluate_expression_part(std::shared_ptr<Branch> value_branch)
{
    std::string value = "";
    std::string branch_type = value_branch->getType();
    std::string branch_value = value_branch->getValue();
    if (branch_type == "identifier")
    {
        // Ok we need to get the value of this definition name that was provided for the value
        value = get_definition_value(branch_value);
    }
    else if (branch_type == "number" 
            || branch_type == "string")
    {
        value = branch_value;
    }
    else
    {
        throw Exception("std::string Preprocessor::evaluate_expression_part(std::shared_ptr<Branch> value_branch): expecting either an identifier, string or a number token");
    }

    return value;
}

bool Preprocessor::is_string_numeric_only(std::string str)
{
    for (char c : str)
    {
        if (!isdigit(c) && c != '-')
            return false;
    }

    return true;
}