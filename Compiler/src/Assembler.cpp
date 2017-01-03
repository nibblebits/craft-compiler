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
 * File:   Assembler.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 16:13
 * 
 * Description: 
 */

#include "Assembler.h"
#include "EBranch.h"

const char operators[] = {'=', '+', '-', '/', '*', '<', '>', '&', '|', '^', '%', '!'};
const char symbols[] = {'(', ')', ',', '#', '{', '}', '.', '[', ']', ';', ':'};

Assembler::Assembler(Compiler* compiler, std::shared_ptr<VirtualObjectFormat> object_format) : CompilerEntity(compiler)
{
    this->object_format = object_format;
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

std::shared_ptr<VirtualObjectFormat> Assembler::getObjectFormat()
{
    return this->object_format;
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
            else if (isRegister(tokenValue))
            {
                lex_token = new Token("register", tokenValue, position);
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

void Assembler::addRegister(std::string _register)
{
    this->registers.push_back(_register);
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
    if ((op >= 65 && op <= 90) || (op >= 97 && op <= 122) || op == 95 || op == '_')
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

bool Assembler::isRegister(std::string op)
{
    for (std::string c : this->registers)
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

void Assembler::pop_front_branch()
{
    if (this->branches.empty())
    {
        throw AssemblerException("void Assembler8086::pop_front_branch(): the stack is empty.");
    }

    this->branch = this->branches.front();
    this->branch_type = this->branch->getType();
    this->branch_value = this->branch->getValue();
    this->branches.pop_front();
}

void Assembler::peek()
{
    peek(0);
}

void Assembler::peek(int offset)
{
    if (!this->tokens.empty())
    {
        if (offset == -1)
        {
            this->peek_token = this->tokens.front();
        }
        else
        {
            if (offset < this->tokens.size())
            {
                this->peek_token = this->tokens.at(offset);
            }
            else
            {
                goto _peek_error;
            }
        }
        this->peek_token_type = this->peek_token->getType();
        this->peek_token_value = this->peek_token->getValue();
    }
    else
    {

        goto _peek_error;
    }

    return;

_peek_error:
    // We will just null the peek here.
    this->peek_token = NULL;
    this->peek_token_type = "";
    this->peek_token_value = "";
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
    if (getpeekTokenType() == type)
        return true;

    return false;
}

bool Assembler::is_peek_value(std::string value)
{
    if (getpeekTokenValue() == value)
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

std::shared_ptr<Branch> Assembler::sum_expression(std::shared_ptr<Branch> expression_branch)
{
    if (expression_branch->getType() != "E")
    {
        // Not an expression branch, nothing to do
        return expression_branch;
    }

    std::shared_ptr<Branch> new_branch = std::shared_ptr<EBranch>(new EBranch(getCompiler(), "+"));
    // Clone the expression branch we want a new instance
    expression_branch = expression_branch->clone();
    // Add it as a child of our new branch
    new_branch->addChild(expression_branch);

    CharPos last_char_pos;
    std::string op;

    // Shorten the expression branch.
    int sum = 0;
    std::shared_ptr<EBranch> e_branch = std::dynamic_pointer_cast<EBranch>(expression_branch);
    e_branch->iterate_expressions([&](std::shared_ptr<EBranch> root_e, std::shared_ptr<Branch> left_branch, std::shared_ptr<Branch> right_branch) -> void
    {
        op = root_e->getValue();
        if (left_branch->getType() == "number"
                && right_branch->getType() == "number")
        {
            std::shared_ptr<Token> r_token = std::dynamic_pointer_cast<Token>(right_branch);
            last_char_pos = r_token->getPosition();

            sum += getCompiler()->evaluate(std::stoi(left_branch->getValue()), std::stoi(right_branch->getValue()), op);
            // Ok now remove the branch
            root_e->removeSelf();
        }
        else if(left_branch->getType() == "number"
                || right_branch->getType() == "number")
        {
            std::shared_ptr<Token> target_token;
            if (left_branch->getType() == "number")
            {
                target_token = std::dynamic_pointer_cast<Token>(left_branch);
            }
            else
            {
                target_token = std::dynamic_pointer_cast<Token>(right_branch);
            }
            
            // It is only save to evaluate "+" and "-" as both branches are not numbers
            if (op == "+"
                    || op == "-")
            {
                sum = getCompiler()->evaluate(sum, std::stoi(target_token->getValue()), op);
                target_token->removeSelf();
                root_e->rebuild();
            }
            
        }
    });

    // Finally we now need to create a new branch for the sum and add it on
    std::shared_ptr<Token> summed_branch = std::shared_ptr<Token>(new Token("number", std::to_string(sum), last_char_pos));
    if (new_branch->getChildren().size() == 0)
    {
        // Zero children just return the summed token branch
        return summed_branch;
    }
    
    // Ok the new branch has a child so lets add the summed branch and return
    new_branch->addChild(summed_branch);
    return new_branch;
}

/* NOTE: BODMAS, order of operations currently does not apply, this should be added in the future. */
void Assembler::parse_expression(std::shared_ptr<Branch> left_branch)
{
    std::shared_ptr<Branch> op = NULL;
    std::shared_ptr<Branch> right_branch = NULL;

    if (left_branch == NULL)
    {
        // Peek futher to see if this is a new expression or not
        peek();
        if (is_peek_symbol("("))
        {
            // Its a new expression so shift and pop off the left bracket "("
            shift_pop();
            parse_expression();
            pop_branch();
            left_branch = getPoppedBranch();

            // Ok now shift and pop off the right bracket ")"
            shift_pop();
        }
        else
        {
            // Get the left expression
            this->left_exp_handler();
            pop_branch();
            left_branch = getPoppedBranch();
        }
    }
    peek();
    if (is_peek_type("operator"))
    {
        // Shift and pop the operator
        shift_pop();
        op = getPoppedBranch();

        // Peek futher to see if this is a new expression or not
        peek();
        if (is_peek_symbol("("))
        {
            // Its a new expression so shift and pop off the left bracket "("
            shift_pop();
            parse_expression();
            pop_branch();
            right_branch = getPoppedBranch();

            // Ok now shift and pop off the right bracket ")"
            shift_pop();
        }
        else
        {
            // Get the right expression
            this->right_exp_handler();
            pop_branch();
            right_branch = getPoppedBranch();
        }
    }

    if (op != NULL)
    {
        // This is an expression, create an expression branch
        std::shared_ptr<EBranch> exp_branch = std::shared_ptr<EBranch>(new EBranch(getCompiler(), op->getValue()));
        exp_branch->addChild(left_branch);
        exp_branch->addChild(right_branch);

        // Peek further to see if we are still continuing
        peek();
        if (is_peek_type("operator"))
        {
            // Ok we have more
            parse_expression(exp_branch);
        }
        else
        {
            // No nothing else so push the expression branch
            push_branch(exp_branch);
        }
    }
    else
    {
        // There is no operator here this is not an expression just push the left.
        push_branch(left_branch);
    }

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

std::shared_ptr<Token> Assembler::getpeekToken()
{
    return this->peek_token;
}

std::string Assembler::getpeekTokenType()
{
    return this->peek_token_type;
}

std::string Assembler::getpeekTokenValue()
{
    return this->peek_token_value;
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