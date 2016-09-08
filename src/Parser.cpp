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
 * File:   Parser.cpp
 * Author: Daniel McCarthy
 *
 * Created on 29 May 2016, 20:31
 * 
 * Description: Takes token input and converts it to an AST(Abstract Syntax Tree)
 * 
 */

#include "Parser.h"
#include "branches.h"

Parser::Parser(Compiler* compiler) : CompilerEntity(compiler)
{
    this->tree = std::shared_ptr<Tree>(new Tree());
    this->logger = std::shared_ptr<Logger>(new Logger());
    this->token = NULL;

}

Parser::~Parser()
{

}

void Parser::setInput(std::vector<std::shared_ptr<Token>> tokens)
{
    // Push the tokens to the input stack.
    for (std::shared_ptr<Token> token : tokens)
    {
        this->input.push_back(token);
    }
}

/* 
 * \brief Processes the root of the input
 * 
 * Processes the top of the input, the root is where a programmer would declare global variables and functions
 */
void Parser::process_top()
{
    peak();
    if (this->peak_token_type == "keyword")
    {
        peak(1);
        if (this->peak_token_type == "identifier")
        {
            // Check to see if this is a function or a variable declaration
            peak(2);
            if (is_peak_symbol(";"))
            {
                // The peak ends with a semicolon so it must be a variable declaration

                process_variable_declaration();

                // Shift and pop the semicolon off the stack. This is safe to do as it is not yet on the branch stack
                shift_pop();
            }
            else
            {
                process_function();
            }
        }
    }
    else
    {
        error_unexpected_token();
    }
}

/* 
 * \brief Processes the function of the input
 * 
 */
void Parser::process_function()
{
    // Pop the function name and return type from the stack
    shift_pop();
    std::shared_ptr<Branch> func_return_type = this->branch;
    shift_pop();
    std::shared_ptr<Branch> func_name = this->branch;

    // Shift and pop the next symbol we need to make sure its a left bracket
    this->shift_pop();
    if (!is_branch_symbol("("))
    {
        error_expecting("(", this->branch_value);
    }

    std::shared_ptr<Branch> func_arguments = std::shared_ptr<Branch>(new Branch("FUNC_ARGUMENTS", ""));
    // Process all the function parameters
    while (true)
    {
        /* If the next token is a keyword then process a variable declaration*/
        peak();
        if (this->peak_token_type == "keyword")
        {
            process_variable_declaration();

            // Pop the resulting variable declaration and put it in the function arguments branch
            pop_branch();
            func_arguments->addChild(this->branch);
        }
        else
        {
            /* Its not a keyword so check for a right bracket or a comma*/
            if (is_peak_symbol(")"))
            {
                // Looks like we are done here so shift and pop the bracket from the stack then break
                shift_pop();
                break;
            }
            else if (is_peak_symbol(","))
            {
                // shift and pop the comma from the stack
                shift_pop();
            }
            else
            {
                // Neither were provided we have a syntax error
                error_expecting(", or )", this->branch_value);
                break;
            }
        }
    }


    // Process the function body
    process_body();

    // Pop off the body
    pop_branch();

    std::shared_ptr<Branch> body = this->branch;

    // Finally create the function branch and merge it all together
    std::shared_ptr<Branch> func_branch = std::shared_ptr<Branch>(new Branch("FUNC", ""));
    func_branch->addChild(func_return_type);
    func_branch->addChild(func_name);
    func_branch->addChild(func_arguments);
    func_branch->addChild(body);

    // Now push it back to the stack
    push_branch(func_branch);
}

void Parser::process_body()
{
    // Check that the next branch is a left bracket all bodys must start with them.
    shift_pop();
    if (!is_branch_symbol("{"))
    {
        error_expecting("{", this->branch_value);
    }

    std::shared_ptr<Branch> body_root = std::shared_ptr<Branch>(new Branch("BODY", ""));

    while (true)
    {
        // Check to see if we are at the end of the body
        peak();
        if (is_peak_symbol("}"))
        {
            // Shift and pop the right bracket
            shift_pop();

            // We are done.
            break;
        }
        else
        {
            // Nope so process the statement
            process_stmt();

            // Pop off the result and store it in the body_root
            pop_branch();
            body_root->addChild(this->branch);
        }
    }

    // Push the body root onto the branch stack
    push_branch(body_root);
}

// All possible body statements

void Parser::process_stmt()
{
    peak();
    if (this->peak_token_type == "keyword")
    {
        // Has to be a variable or its a syntax error
        peak(1);
        if (this->peak_token_type == "identifier")
        {
            // Its a variable
            process_variable_declaration();
            // Shift the semicolon onto the stack and then pop it off
            shift_pop();
        }
        else
        {
            error_expecting("identifier", this->peak_token_value);
        }
    }
    else if (this->peak_token_type == "identifier")
    {
        peak(1);
        if (this->peak_token_type == "operator")
        {
            // This is an assignment
            process_assignment();

            // Shift the semicolon onto the stack and then pop it off
            shift_pop();
        }
        else
        {
            error("expecting an assignment or function call");
        }
    }
    else
    {
        error_unexpected_token();
    }
}

/* 
 * \brief Processes a variable of the input
 * 
 */
void Parser::process_variable_declaration()
{
    std::shared_ptr<Branch> var_name;
    std::shared_ptr<Branch> var_keyword;

    // Shift the keyword and identifier of the parameter on to the stack
    shift_pop();
    if (branch_type != "keyword")
    {
        error_expecting("keyword", this->branch_value);
    }

    var_keyword = this->branch;

    shift_pop();
    if (branch_type != "identifier")
    {
        error_expecting("identifier", this->branch_value);
    }
    var_name = this->branch;

    /* 
     * Now that we have popped to variable name and keyword of the variable
     * we need to create a branch for, lets create a branch for them  */

    std::shared_ptr<Branch> var_root = std::shared_ptr<Branch>(new Branch("VDEF", ""));
    var_root->addChild(var_keyword);
    var_root->addChild(var_name);

    // Push that root back to the branches
    push_branch(var_root);

}

void Parser::process_assignment()
{
    shift_pop();
    if (this->branch_type != "identifier")
    {
        error_expecting("identifier", this->branch_type);
    }

    std::shared_ptr<Branch> var_name = this->branch;

    shift_pop();
    if (this->branch_type != "operator")
    {
        error_expecting("operator", this->branch_type);
    }
    else
    {
        if (this->branch_value != "=")
        {
            error("assignments must only use the equal sign '='");
        }
    }

    std::shared_ptr<Branch> a_operator = this->branch;
    
    shift_pop();
    if (this->branch_type != "number")
    {
        // Temporary
        error("variables can only be assigned to numbers");
    }

    std::shared_ptr<Branch> value = this->branch;
    
    std::shared_ptr<Branch> assign_root = std::shared_ptr<Branch>(new Branch("ASSIGN", ""));
    assign_root->addChild(var_name);
    assign_root->addChild(a_operator);
    assign_root->addChild(value);
    
    // Finally push the assign root to the stack
    push_branch(assign_root);
    
}

void Parser::error(std::string message, bool token)
{
    if (token)
    {
        CharPos position = this->token->getPosition();
        message += " on line " + std::to_string(position.line_no) + ", col:" + std::to_string(position.col_pos);
    }
    this->logger->error(message);

    throw ParserException("Error with source cannot continue");
}

void Parser::warn(std::string message, bool token)
{
    if (token)
    {
        CharPos position = this->token->getPosition();
        message += " on line " + std::to_string(position.line_no) + ", col:" + std::to_string(position.col_pos);
    }
    this->logger->warn(message);
}

void Parser::error_unexpected_token()
{
    error("Unexpected token: " + this->token_value + " maybe you have forgot a semicolon? ';'");
}

void Parser::error_expecting(std::string expecting, std::string given)
{
    error("Expecting: '" + expecting + "' but '" + given + "' was given");
}

void Parser::shift()
{
    if (!this->input.empty())
    {
        this->token = this->input.front();
        this->token_type = this->token->getType();
        this->token_value = this->token->getValue();
        push_branch(this->token);
        this->input.pop_front();
    }
    else
    {

        error("no more input with unfinished parse", false);
        throw ParserException("End of file reached.");
    }
}

void Parser::peak(int offset)
{
    if (!this->input.empty())
    {
        if (offset == -1)
        {
            this->peak_token = this->input.front();
        }
        else
        {
            if (offset < this->input.size())
            {
                this->peak_token = this->input.at(offset);
            }
            else
            {
                goto _peak_error;
            }
        }
        this->peak_token_type = this->peak_token->getType();
        this->peak_token_value = this->peak_token->getValue();
    }
    else
    {

        goto _peak_error;
    }

    return;

_peak_error:
    error("peek failed, no more input with unfinished parse", false);
    throw ParserException("End of file reached.");

}

void Parser::pop_branch()
{
    if (!this->branches.empty())
    {
        this->branch = this->branches.back();
        this->branch_type = this->branch->getType();
        this->branch_value = this->branch->getValue();
        this->branches.pop_back();
    }
    else
    {
        error("no more branches on the stack", false);
        throw ParserException("No branches on the stack");
    }
}

void Parser::push_branch(std::shared_ptr<Branch> branch)
{
    this->branches.push_back(branch);
}

void Parser::shift_pop()
{
    this->shift();
    this->pop_branch();
}

bool Parser::is_branch_symbol(std::string symbol)
{
    return this->branch_type == "symbol" && this->branch_value == symbol;
}

bool Parser::is_peak_symbol(std::string symbol)
{
    return this->peak_token_type == "symbol" && this->peak_token_value == symbol;
}

void Parser::buildTree()
{
    if (this->input.empty())
    {
        throw ParserException("Nothing to parse.");
    }

    while (!this->input.empty())
    {
        this->process_top();
    }

    std::shared_ptr<Branch> root = std::shared_ptr<Branch>(new Branch("root", ""));
    while (!this->branches.empty())
    {
        std::shared_ptr<Branch> branch = this->branches.front();
        root->addChild(branch);
        this->branches.pop_front();
    }
    this->tree->root = root;

}

std::shared_ptr<Tree> Parser::getTree()
{
    return this->tree;
}

std::shared_ptr<Logger> Parser::getLogger()
{
    return this->logger;
}