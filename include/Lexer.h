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
 * File:   Lexer.h
 * Author: Daniel McCarthy
 *
 * Created on 27 May 2016, 16:39
 */

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "Token.h"
#include "LexerException.h"

typedef bool (*is_func)();
typedef bool (*callback_func)(char);

class Lexer
{
public:
    Lexer();
    void setInput(std::string input);
    void tokenize();
    std::vector<Token*> getTokens();
    virtual ~Lexer();
private:
    std::string input;
    std::vector<Token*> tokens;
    Token* token;
    std::string tokenValue;
    CharPos position;
    std::string::iterator it;
    void fillTokenWhile(callback_func callback);
    
    static bool isOperator(char op);
    static bool isSymbol(char op);
    static bool isCharacter(char op);
    static bool isNumber(char op);
    static bool isWhitespace(char op);
    static bool isKeyword(std::string op);
    

};

#endif /* LEXER_H */

