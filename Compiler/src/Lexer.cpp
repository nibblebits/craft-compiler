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
 * File:   Lexer.cpp
 * Author: Daniel McCarthy
 *
 * Created on 27 May 2016, 16:39
 * 
 * Description: Turns input into a series of Tokens to be later processed by the parser.
 */

#include <iostream>
#include "Lexer.h"
#include "Compiler.h"

const char operators[] = {'=', '+', '-', '/', '*', '<', '>', '&', '|', '^', '%', '!'};
const char symbols[] = {'(', ')', ',', '#', '{', '}', '.', '[', ']', ';'};
const std::string general_keywords[] = {
    "if", "for", "do", "while", "continue", "break", "continue", "switch", "__asm", "string", "else", "return", "ifdef", "define"
};

const std::string data_type_keywords[] = {
    "nibble", "bit", "uint8", "int8", "uint16", "int16", "uint32", "int32", "uint64", "int64",
    "struct", "void"
};

Lexer::Lexer(Compiler* compiler) : CompilerEntity(compiler)
{

}

Lexer::~Lexer()
{

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

    token = NULL;
    tokenValue = "";
    position.line_no = 1;
    position.col_pos = 1;

    for (it = this->input.begin(); it < this->input.end(); it++)
    {
        char c = *it;
        if (isComment())
        {
            if (isLineComment())
            {
                ignore_line();
            }
            else
            {
                /* This must be a multi-line comment 
                 * lets ignore the first two bytes as they are
                 * "/*" */
                it += 2;
                // Now we need to ignore until we see "*/"
                std::string dummyValue = "";
                bool op_found = false;
                fillTokenWhile([&](char c) -> bool
                {
                    if (op_found)
                    {
                        if (c == '/')
                        {
                            // Ok we are at the end
                            return false;
                        }
                    }
                    else if (c == '*')
                    {
                        op_found = true;
                    }

                    return true;
                }, &dummyValue);
                // Ok finally lets ignore the last "*/"
                it += 2;
            }
        }
        else if (isCharacter(c))
        {
            fillTokenWhile([](char c) -> bool
            {
                return isCharacter(c) || isNumber(c);
            });

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
            bool first_done = false;
            fillTokenWhile([&](char c) -> bool
            {
                // Required as pointers need multiple operator tokens not one as a whole
                if (first_done && c == '*')
                    return false;
                first_done = true;
                return isOperator(c);
            });

            token = new Token("operator", tokenValue, position);
        }
        else if (isSymbol(c))
        {
            tokenValue = *it;
            token = new Token("symbol", tokenValue, position);
        }
        else if (isNumber(c))
        {
            // We need to check for formatting here, maybe it is hex or maybe its binary
            char c2 = *(it + 1);
            if (c == '0' && (c2 == 'x' || c2 == 'b'))
            {
                // We need to add two bytes to the iterator as we no longer care about the 0x or 0b values
                it += 2;
                // Ok there is formatting so this is a hex or binary number
                fillTokenWhile([](char c) -> bool
                {
                    return isNumber(c)
                            || isCharacter(c);
                });

                try
                {
                    // Ok we now have the formatted string so lets convert it to a decimal value as a string and assign it as the token value
                    tokenValue = std::to_string(getCompiler()->getNumberFromString(tokenValue, c2));
                }
                catch (Exception &ex)
                {
                    throw LexerException(position, "a problem occurred while formatting your number: " + ex.getMessage());
                }

            }
            else
            {
                fillTokenWhile([](char c) -> bool
                {
                    return isNumber(c);
                });
            }
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
            }
            while (true);
            token = new Token("string", tokenValue, position);
        }
        else
        {
            throw LexerException(position, "an invalid character was found '" + std::to_string(c) + "d' are you sure this is not a binary file?");
        }

        // Reset the token value and push the token to the tokens vector
        if (token != NULL)
        {

            tokenValue = "";
            std::shared_ptr<Token> token_sp(token);
            tokens.push_back(token_sp);
            token = NULL;
        }

        position.col_pos++;
    }
}

std::vector<std::shared_ptr<Token>> Lexer::getTokens()
{
    return this->tokens;
}

void Lexer::fillTokenWhile(std::function<bool(char c) > callback, std::string* custom_tokenValue)
{
    if (custom_tokenValue == NULL)
    {
        // No custom token value was provided so lets default to the default token value
        custom_tokenValue = &this->tokenValue;
    }

    char c;
    *custom_tokenValue = "";
    do
    {
        c = PeekNextChar(&it);
        if (callback(c))
        {
            *custom_tokenValue += c;
            position.col_pos++;
            it++;
        }
        else
        {
            it--;
            break;
        }
    }

    while (true);
}

char Lexer::PeekNextChar(std::string::iterator* custom_iterator)
{
    if (*custom_iterator < this->input.end())
    {
        char v = **custom_iterator;
        return v;
    }

    throw Exception("No more input ensure comments are closed (if_any).", "char Lexer::PeekNextChar(std::string::iterator* custom_iterator)");
}

/**
 * Attempts to read a character from the custom_iterator
 * then increments the custom_iterator
 * 
 * @param custom_iterator
 * @throws Throws an exception if out of bounds
 * @return the next character for the custom_iterator
 */
char Lexer::ReadNextChar(std::string::iterator* custom_iterator)
{
    char c = PeekNextChar(custom_iterator);
    *custom_iterator++;
    return c;
}

void Lexer::ignore_line()
{
    std::string dummy_value;
    // Ignore all characters until a new line is found.
    fillTokenWhile([](char c) -> bool
    {
        return c != 10;
    }, &dummy_value);

    // We don't really want a new line terminator left for us so lets ignore it too
    ReadNextChar(&it);
}

bool Lexer::isComment()
{
    return isLineComment() || isMultiLineComment();
}

bool Lexer::isLineComment()
{
    std::string::iterator it_b = it;
    if (*it_b == '/' && *(it_b + 1) == '/')
    {
        return true;
    }
    return false;
}

bool Lexer::isMultiLineComment()
{
    std::string::iterator it_b = it;
    if (*it_b == '/' && *(it_b + 1) == '*')
    {
        return true;
    }
    return false;
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
    if ((op >= 65 && op <= 90) || (op >= 97 && op <= 122) || op == 95)

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
    for (std::string c : data_type_keywords)
    {
        if (c == op)
            return true;
    }

    for (std::string c : general_keywords)
    {
        if (c == op)

            return true;
    }

    return false;
}

bool Lexer::isDataTypeKeyword(std::string value)
{
    for (std::string c : data_type_keywords)
    {
        if (c == value)
            return true;
    }

    return false;
}