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
 * File:   Lexer.cpp
 * Author: Daniel McCarthy
 *
 * Created on 27 May 2016, 16:39
 * 
 * Description: Turns input into a series of Tokens to be later processed by the parser.
 */

#include "Lexer.h"
#include <iostream>
const char operators[] = {'+', '-', '/', '*'};
const char symbols[] = {'=', '(', ')', ';'};
const std::string keywords[] = {
    "nibble", "bit", "uint8", "int8", "uint16", "int16", "uint32", "int32", "uint64", "int64",
    "if", "for", "do", "while", "next", "continue", "break", "switch"
};

Lexer::Lexer()
{
}

Lexer::~Lexer()
{
    // Delete all the tokens they are no longer needed
    for (Token* token : this->tokens)
    {
        delete token;
    }
}

void Lexer::setInput(std::string input)
{
    this->input = input;
}

void Lexer::tokenize()
{
    if (this->input.length() == 0)
    {
        throw LexerException("No input has been provided.");
    }

    Token* token = NULL;
    std::string tokenValue;
    CharPos position;
    position.line_no = 1;
    position.col_pos = 1;

    for (std::string::iterator it = this->input.begin(); it < this->input.end(); it++)
    {
        char c = *it;
        if (isCharacter(c))
        {
            tokenValue = c;
            do
            {
                it++;
                c = *it;
                if (isCharacter(c) || isNumber(c))
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


            if (isKeyword(tokenValue))
            {
                token = new Token("keyword", tokenValue, position);
            }
            else
            {
                token = new Token("identifier", tokenValue, position);
            }
        }
        else if (isOperator(c))
        {
            tokenValue = c;
            do
            {
                it++;
                c = *it;
                if (isOperator(c))
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

            token = new Token("operator", tokenValue, position);
        }
        else if (isSymbol(c))
        {
            tokenValue = c;
            do
            {
                it++;
                c = *it;
                if (isSymbol(c))
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

            token = new Token("symbol", tokenValue, position);
        }
        else if (isNumber(c))
        {
            tokenValue = c;
            do
            {
                it++;
                c = *it;
                if (isNumber(c))
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

            token = new Token("number", tokenValue, position);
        }
        else if (isWhitespace(c))
        {
            if (c == '\n')
            {
                position.line_no++;
                position.col_pos = 0;
            }
        }
        else if (c == '"')
        {
            // A string has been opened
            do
            {
                it++;
                c = *it;
                if (c != '"')
                {
                    tokenValue += c;
                    position.col_pos++;
                }
                else
                {
                    break;
                }

                token = new Token("string", tokenValue, position);
            }
            while (true);
        }
        else
        {
            throw LexerException(position, "an invalid character was found '" + std::to_string(c) + "d' are you sure this is not a binary file?");
        }

        // Reset the token value and push the token to the tokens vector
        if (token != NULL)
        {
            tokenValue = "";
            tokens.push_back(token);
            token = NULL;
        }

        position.col_pos++;
    }
}

std::vector<Token*> Lexer::getTokens()
{
    return this->tokens;
}

bool Lexer::isOperator(char op)
{
    for (char c : operators)
    {
        if (c == op)
            return true;
    }

    return false;
}

bool Lexer::isSymbol(char op)
{
    for (char c : symbols)
    {
        if (c == op)
            return true;
    }
    return false;
}

bool Lexer::isCharacter(char op)
{
    if ((op >= 65 && op <= 90) || (op >= 97 && op <= 122))
        return true;
    return false;
}

bool Lexer::isNumber(char op)
{
    if (op >= 48 && op <= 57)
        return true;
    return false;
}

bool Lexer::isWhitespace(char op)
{
    return (op < 33);
}

bool Lexer::isKeyword(std::string op)
{
    for (std::string c : keywords)
    {
        if (c == op)
            return true;
    }

    return false;
}


