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
 * File:   Lexer.h
 * Author: Daniel McCarthy
 *
 * Created on 27 May 2016, 16:39
 */

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Token.h"
#include "LexerException.h"
#include "CompilerEntity.h"
#include "def.h"
typedef bool (*is_func)();

class EXPORT Lexer : public CompilerEntity
{
public:
    Lexer(Compiler* compiler, std::string filename="");
    void setFilename(std::string filename);
    void setInput(std::string input);
    void tokenize();
    std::vector<std::shared_ptr<Token>> getTokens();
    static bool isDataTypeKeyword(std::string value);
    virtual ~Lexer();
private:
    std::string input;
    std::vector<std::shared_ptr<Token>> tokens;
    Token* token;
    std::string tokenValue;
    CharPos position;
    std::string filename;
    std::string::iterator it;

    void fillTokenWhile(std::function<bool(char c) > callback, std::string* custom_tokenValue = NULL);
    char HandleEscapeSequence(char c);
    
    char PeekNextChar(std::string::iterator* custom_iterator);
    // Reads the next character from the input string but also checks bounds against the iterator
    char ReadNextChar(std::string::iterator* custom_iterator);

    void ignore_line();

    bool isComment();
    bool isLineComment();
    bool isMultiLineComment();

    static bool isOperator(char op);
    static bool isSymbol(char op);
    static bool isCharacter(char op);
    static bool isNumber(char op);
    static bool isWhitespace(char op);
    static bool isKeyword(std::string op);


};

#endif /* LEXER_H */

