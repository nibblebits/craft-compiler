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
 * File:   Assembler.h
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 16:13
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H


#include <memory>
#include <deque>
#include <vector>
#include <string>

#include "Branch.h"
#include "Token.h"
#include "RootBranch.h"
#include "AssemblerException.h"
#include "CompilerEntity.h"
typedef bool (*is_func)();
typedef bool (*callback_func)(char);

class EXPORT Assembler : public CompilerEntity
{
public:
    Assembler(Compiler* compiler);
    virtual ~Assembler();

    void setInput(std::string input);
    void run();
protected:
    void lexify();
    virtual std::shared_ptr<Branch> parse() = 0;
    virtual void generate() = 0;
    virtual void left_exp_handler() = 0;
    virtual void right_exp_handler() = 0;
    void setCommentSymbol(unsigned char comment_symb);
    void addKeyword(std::string keyword);
    void addInstruction(std::string instruction);
    std::vector<std::shared_ptr<Token>> getTokens();

    void push_branch(std::shared_ptr<Branch> branch);
    void pop_branch();
    void pop_front_branch();
    void peek();
    void peek(int offset);
    void shift();
    void shift_pop();

    bool is_peek_type(std::string type);
    bool is_peek_value(std::string value);
    bool is_peek_operator(std::string op);
    bool is_peek_symbol(std::string op);
    bool is_peek_identifier(std::string iden);
    bool is_peek_keyword(std::string keyword);
    bool is_peek_instruction(std::string ins);

    bool is_popped_type(std::string type);
    bool is_popped_value(std::string value);
    bool is_popped_operator(std::string op);
    bool is_popped_symbol(std::string op);
    bool is_popped_identifier(std::string iden);
    bool is_popped_keyword(std::string keyword);
    bool is_popped_instruction(std::string ins);

    void parse_expression(std::shared_ptr<Branch> left_branch=NULL);
    
    std::shared_ptr<Token> getShiftedToken();
    std::string getShiftedTokenType();
    std::string getShiftedTokenValue();

    std::shared_ptr<Token> getPeakToken();
    std::string getPeakTokenType();
    std::string getPeakTokenValue();

    std::shared_ptr<Branch> getPoppedBranch();
    std::string getPoppedBranchType();
    std::string getPoppedBranchValue();
    
    bool hasTokens();
    bool hasBranches();

private:

    void fillTokenWhile(callback_func callback);
    static bool isOperator(char op);
    static bool isSymbol(char op);
    static bool isCharacter(char op);
    static bool isNumber(char op);
    static bool isWhitespace(char op);
    bool isKeyword(std::string op);
    bool isInstruction(std::string op);

    std::shared_ptr<Token> token;
    std::string token_type;
    std::string token_value;

    std::shared_ptr<Token> peak_token;
    std::string peak_token_type;
    std::string peak_token_value;

    std::shared_ptr<Branch> branch;
    std::string branch_type;
    std::string branch_value;

    std::string input;
    std::string::iterator it;
    unsigned char comment_symb;
    std::vector<std::string> keywords;
    std::vector<std::string> instructions;
    Token* lex_token;
    std::string tokenValue;
    CharPos position;
    
    // Tokens variable will be corrupted after parsing therefore a tokens vector maintaining original result is required.
    std::vector<std::shared_ptr<Token>> tokens_vec;
    std::deque<std::shared_ptr<Token>> tokens;
    std::deque<std::shared_ptr<Branch>> branches;

    std::shared_ptr<RootBranch> root;
};

#endif /* ASSEMBLER_H */

