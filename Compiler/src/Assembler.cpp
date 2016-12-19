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
 * File:   Assembler.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 16:13
 * 
 * Description: 
 */

#include "Assembler.h"

const char operators[] = {'=', '+', '-', '/', '*', '<', '>', '&', '|', '^', '%', '!'};
const char symbols[] = {'(', ')', ',', '#', '{', '}', '.', '[', ']', ';', ':'};

Assembler::Assembler(Compiler* compiler) : CompilerEntity(compiler)
{
    setCommentSymbol(';');
}

Assembler::~Assembler()
{
}

void Assembler::setInput(std::string input)
{
    this->input = input;
}

void Assembler::run()
{
    lexify();
    parse();
    generate();
}

void Assembler::lexify()
{
    lex_token = NULL;
    tokenValue = "";
    position.line_no = 1;
    position.col_pos = 1;

    for (it = this->input.begin(); it < this->input.end(); it++)
    {
        char c = *it;
        if (c == this->comment_symb)
        {
            // This is a comment ignore the entire line
            do
            {
                it++;
                c = *it;
                if (c == '\n')
                    break;
            }
            while (true);
        }
        else if (isCharacter(c))
        {
            fillTokenWhile([](char c) -> bool
            {
                return isCharacter(c) || isNumber(c);
            });

            if (isKeyword(tokenValue))
            {
                lex_token = new Token("keyword", tokenValue, position);
            }
            else if (isInstruction(tokenValue))
            {
                lex_token = new Token("instruction", tokenValue, position);
            }
            else
            {
                lex_token = new Token("identifier", tokenValue, position);
            }
        }
        else if (isOperator(c))
        {
            fillTokenWhile([](char c) -> bool
            {
                return isOperator(c);
            });

            lex_token = new Token("operator", tokenValue, position);
        }
        else if (isSymbol(c))
        {
            tokenValue = *it;
            lex_token = new Token("symbol", tokenValue, position);
        }
        else if (isNumber(c))
        {
            tokenValue = c;
            fillTokenWhile([](char c) -> bool
            {
                return isNumber(c);
            });

            lex_token = new Token("number", tokenValue, position);
        }
        else if (isWhitespace(c))
        {
            if (c == '\n')
            {
                position.line_no++;
                position.col_pos = 0;
            }
        }
        else if (c == '\'')
        {
            // A string has been opened
            do
            {
                it++;
                c = *it;
                if (c != '\'')
                {
                    tokenValue += c;
                    position.col_pos++;
                }
                else
                {
                    break;
                }
            }
            while (true);
            lex_token = new Token("string", tokenValue, position);
        }
        else
        {
            throw AssemblerException(position, "an invalid character was found when assembling, the character is '" + std::to_string(c) + "d', make sure there is not an issue with the code generator.");
        }

        // Reset the token value and push the token to the tokens vector
        if (lex_token != NULL)
        {
            tokenValue = "";
            std::shared_ptr<Token> token_sp(lex_token);
            tokens.push_back(token_sp);
            // Have to also push to a tokens vector as the tokens deque is corrupted after parsing.
            tokens_vec.push_back(token_sp);
            lex_token = NULL;
        }
        position.col_pos++;
    }

}

void Assembler::setCommentSymbol(unsigned char comment_symb)
{
    this->comment_symb = comment_symb;
}

void Assembler::addKeyword(std::string keyword)
{
    this->keywords.push_back(keyword);
}

void Assembler::addInstruction(std::string instruction)
{
    this->instructions.push_back(instruction);
}

std::vector<std::shared_ptr<Token>> Assembler::getTokens()
{
    return this->tokens_vec;
}

void Assembler::fillTokenWhile(callback_func callback)
{
    char c = *it;
    tokenValue = c;
    do
    {
        it++;
        c = *it;
        if (callback(c))
        {
            tokenValue += c;
            position.col_pos++;
        }
        else
        {
            it--;
            break;
        }
    }
    while (true);
}

bool Assembler::isOperator(char op)
{
    for (char c : operators)
    {
        if (c == op)
            return true;
    }

    return false;
}

bool Assembler::isSymbol(char op)
{
    for (char c : symbols)
    {
        if (c == op)
            return true;
    }
    return false;
}

bool Assembler::isCharacter(char op)
{
    if ((op >= 65 && op <= 90) || (op >= 97 && op <= 122) || op == 95)
        return true;
    return false;
}

bool Assembler::isNumber(char op)
{
    if (op >= 48 && op <= 57)
        return true;
    return false;
}

bool Assembler::isWhitespace(char op)
{
    return (op < 33);
}

bool Assembler::isKeyword(std::string op)
{
    for (std::string c : this->keywords)
    {
        if (c == op)
            return true;
    }

    return false;
}

bool Assembler::isInstruction(std::string op)
{
    for (std::string c : this->instructions)
    {
        if (c == op)
            return true;
    }

    return false;
}

void Assembler::push_branch(std::shared_ptr<Branch> branch)
{
    this->branches.push_back(branch);
}

void Assembler::pop_branch()
{
    if (this->branches.empty())
    {
        throw AssemblerException("void Assembler8086::pop_branch(): the stack is empty.");
    }

    this->branch = this->branches.back();
    this->branch_type = this->branch->getType();
    this->branch_value = this->branch->getValue();
    this->branches.pop_back();
}

void Assembler::peek()
{
    peek(0);
}

void Assembler::peek(int offset)
{
    if (!this->input.empty())
    {
        if (offset == -1)
        {
            this->peak_token = this->tokens.front();
        }
        else
        {
            if (offset < this->input.size())
            {
                this->peak_token = this->tokens.at(offset);
            }
            else
            {
                goto _peek_error;
            }
        }
        this->peak_token_type = this->peak_token->getType();
        this->peak_token_value = this->peak_token->getValue();
    }
    else
    {

        goto _peek_error;
    }

    return;

_peek_error:
    throw AssemblerException("void Assembler::peek(): End of file reached.");
}

void Assembler::shift()
{
    if (this->tokens.empty())
    {
        throw AssemblerException("void Assembler8086::shift(): no more input to shift.");
    }

    std::shared_ptr<Token> token = this->tokens.front();
    this->token = token;
    this->token_type = token->getType();
    this->token_type = token->getValue();
    this->tokens.pop_front();
    push_branch(token);
}

void Assembler::shift_pop()
{
    shift();
    pop_branch();
}

bool Assembler::is_peek_type(std::string type)
{
    if (getPeakTokenType() == type)
        return true;

    return false;
}

bool Assembler::is_peek_value(std::string value)
{
    if (getPeakTokenValue() == value)
        return true;
    return false;
}

bool Assembler::is_peek_operator(std::string op)
{
    if (is_peek_type("operator") &&
            is_peek_value(op))
        return true;

    return false;
}

bool Assembler::is_peek_symbol(std::string op)
{
    if (is_peek_type("symbol") &&
            is_peek_value(op))
        return true;

    return false;
}

bool Assembler::is_peek_identifier(std::string iden)
{
    if (is_peek_type("identifier") &&
            is_peek_value(iden))
        return true;

    return false;
}

bool Assembler::is_peek_keyword(std::string keyword)
{
    if (is_peek_type("keyword") &&
            is_peek_value(keyword))
        return true;

    return false;
}

bool Assembler::is_peek_instruction(std::string ins)
{
    if (is_peek_type("instruction") &&
            is_peek_value(ins))
        return true;

    return false;
}

bool Assembler::is_popped_type(std::string type)
{
    if (getPoppedBranchType() == type)
        return true;

    return false;
}

bool Assembler::is_popped_value(std::string value)
{
    if (getPoppedBranchValue() == value)
        return true;
    return false;
}

bool Assembler::is_popped_operator(std::string op)
{
    if (is_popped_type("operator") &&
            is_popped_value(op))
        return true;

    return false;
}

bool Assembler::is_popped_symbol(std::string op)
{
    if (is_popped_type("symbol") &&
            is_popped_value(op))
        return true;

    return false;
}

bool Assembler::is_popped_identifier(std::string iden)
{
    if (is_popped_type("identifier") &&
            is_popped_value(iden))
        return true;

    return false;
}

bool Assembler::is_popped_keyword(std::string keyword)
{
    if (is_popped_type("keyword") &&
            is_popped_value(keyword))
        return true;

    return false;
}

bool Assembler::is_popped_instruction(std::string ins)
{
    if (is_popped_type("instruction") &&
            is_popped_value(ins))
        return true;

    return false;
}

std::shared_ptr<Token> Assembler::getShiftedToken()
{
    return this->token;
}

std::string Assembler::getShiftedTokenType()
{
    return this->token_type;
}

std::string Assembler::getShiftedTokenValue()
{
    return this->token_value;
}

std::shared_ptr<Token> Assembler::getPeakToken()
{
    return this->peak_token;
}

std::string Assembler::getPeakTokenType()
{
    return this->peak_token_type;
}

std::string Assembler::getPeakTokenValue()
{
    return this->peak_token_value;
}

std::shared_ptr<Branch> Assembler::getPoppedBranch()
{
    return this->branch;
}

std::string Assembler::getPoppedBranchType()
{
    return this->branch_type;
}

std::string Assembler::getPoppedBranchValue()
{
    return this->branch_value;
}

bool Assembler::hasTokens()
{
    return !this->tokens.empty();
}

bool Assembler::hasBranches()
{
    return !this->branches.empty();
}