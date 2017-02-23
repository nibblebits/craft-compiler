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
 * File:   Parser.cpp
 * Author: Daniel McCarthy
 *
 * Created on 29 May 2016, 20:31
 * 
 * Description: Takes token input and converts it to an AST(Abstract Syntax Tree)
 * 
 */

#include "Parser.h"
#include "Lexer.h"
#include "branches.h"
#include "common.h"


/* The order of operations for operators and their priorities 
 * Seek here: http://www.difranco.net/compsci/C_Operator_Precedence_Table.htm
 * The same order of operations to C will be used.*/

struct order_of_operation o_of_operation[] = {
    "<<", 0,
    ">>", 0,
    "<", 1,
    ">", 1,
    "<=", 1,
    ">=", 1,
    "==", 2,
    "!=", 2,
    "&", 3,
    "^", 4,
    "|", 5,
    "+", 6,
    "-", 6,
    "*", 7,
    "/", 7,
    "%", 7,
};

Parser::Parser(Compiler* compiler) : CompilerEntity(compiler)
{
    this->tree = std::shared_ptr<Tree>(new Tree());
    this->logger = std::shared_ptr<Logger>(new Logger());
    this->token = NULL;
    this->root_branch = NULL;
    this->root_scope = NULL;
    this->current_local_scope = NULL;
    this->current_function = NULL;
    this->compiler = compiler;
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

void Parser::merge(std::shared_ptr<Branch> root)
{
    /* Lets merge this root with this tree */

    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        push_branch(branch);
    }
}

/* 
 * \brief Processes the root of the input
 * 
 * Processes the top of the input, the root is where a programmer would declare global variables and functions
 */
void Parser::process_top()
{
    this->root_scope = this->root_branch;
    start_local_scope(this->root_branch);
    peek();
    if (is_peek_type("keyword"))
    {
        std::string keyword_value = this->peek_token_value;
        // Is it a structure definition or declaration?
        if (keyword_value == "struct")
        {
            // peek further to see if their is an identifier
            peek(1);
            if (is_peek_type("identifier"))
            {
                // peek further again to see if their is yet another identifier or pointer declaration
                peek(2);
                if (is_peek_type("identifier") 
                        || (is_peek_operator("*") && is_peek_type("identifier", 1)))
                {
                    // This is a structure variable declaration so process it
                    process_structure_declaration();
                    process_semicolon();
                }
                else
                {
                    // This is a structure so process it
                    process_structure();
                }
            }
        }
        else if (keyword_value == "__asm")
        {
            process_inline_asm();
            process_semicolon();
        }
        else
        {
            peek(1);
            if (is_peek_type("identifier"))
            {
                // Check to see if this is a function or a variable declaration
                peek(2);
                if (is_peek_symbol("[") || is_peek_operator("=") || is_peek_symbol(";"))
                {
                    process_variable_declaration();
                    process_semicolon();

                }
                else if (is_peek_symbol("("))
                {
                    // It is looking like a function call.
                    process_function();
                }
                else
                {
                    error_unexpected_token();
                }
            }
            else if (is_peek_operator("*"))
            {
                process_variable_declaration();
                process_semicolon();
            }
        }
    }
    else if (is_peek_symbol("#"))
    {
        // Its a macro so process it
        process_macro();
    }
    else
    {
        error_unexpected_token();
    }

    finish_local_scope();
}

void Parser::process_macro()
{
    // Check that the next token is a hash as all macros require them
    shift_pop();
    if (!is_branch_symbol("#"))
    {
        error_expecting("#", this->branch_value);
    }

    peek();
    if (is_peek_keyword("ifdef"))
    {
        // We have a macro ifdef lets process it
        process_macro_ifdef();
    }
    else if (is_peek_keyword("define"))
    {
        // We have a macro define lets process it
        process_macro_define();
    }
    else if (is_peek_type("identifier"))
    {
        peek(1);
        if (is_peek_symbol("("))
        {
            process_macro_function_call();
        }
        else
        {
            process_macro_definition_identifier();
        }
    }
    else
    {
        error("invalid macro.");
    }
}

void Parser::process_inline_asm()
{
    // Pop the ASM keyword
    shift_pop();
    if (!is_branch_keyword("__asm"))
    {
        error_expecting("__asm", this->branch_value);
    }

    shift_pop();
    if (!is_branch_symbol("("))
    {
        error_expecting("(", this->branch_value);
    }

    shift_pop();
    if (!is_branch_type("string"))
    {
        error_expecting("string", this->branch_type);
    }

    std::shared_ptr<Branch> string_branch = this->branch;
    std::shared_ptr<ASMArgsBranch> asm_args = std::shared_ptr<ASMArgsBranch>(new ASMArgsBranch(this->compiler));
    shift_pop();
    if (is_branch_symbol(","))
    {
        do
        {
            // Process the expression
            process_expression();
            // Pop off the result
            pop_branch();

            std::shared_ptr<ASMArgBranch> asm_arg_branch = std::shared_ptr<ASMArgBranch>(new ASMArgBranch(this->compiler));
            asm_arg_branch->setArgumentValueBranch(this->branch);

            // Store it in the assembly arguments
            asm_args->addChild(asm_arg_branch);

            // Next! 
            shift_pop();
        }
        while (is_branch_symbol(","));
    }

    if (!is_branch_symbol(")"))
    {
        error_expecting(")", this->branch_value);
    }


    // We must now iterate through the given assembly string and modify the branches appropriately, %i signifies an argument must go here.
    std::string string_value = string_branch->getValue();

    // Concat the first string
    std::size_t found = string_value.find("%i", 2);
    if (found != std::string::npos)
    {
        // Update the string branch with the starting string.
        std::string starting_string = string_value.substr(0, found);
        string_branch->setValue(starting_string);

        std::size_t start_offset = found + 2;
        int t_found = 1;
        // Ok brilliant, now its time to handle all the arguments
        for (std::shared_ptr<Branch> child : asm_args->getChildren())
        {
            std::shared_ptr<ASMArgBranch> arg_branch = std::dynamic_pointer_cast<ASMArgBranch>(child);
            std::string next_string = "";
            found = string_value.find("%i", start_offset, 2);
            if (found != std::string::npos)
            {
                std::size_t end_offset = found;
                next_string = string_value.substr(start_offset, end_offset - start_offset);
                start_offset = found + 2;
                t_found++;
            }
            else
            {
                next_string = string_value.substr(start_offset);
            }
            std::shared_ptr<Branch> next_string_branch = std::shared_ptr<Branch>(new Branch("string", next_string));
            arg_branch->setNextStringBranch(next_string_branch);
        }

        // Check for an error
        if (t_found != asm_args->getChildren().size())
        {
            error("string shows more arguments than defined in the __asm arguments");
        }
    }

    std::shared_ptr<ASMBranch> asm_branch = std::shared_ptr<ASMBranch>(new ASMBranch(this->compiler));
    asm_branch->setInstructionStartStringBranch(string_branch);
    asm_branch->setInstructionArgumentsBranch(asm_args);

    // Push the resulting ASM branch to the stack
    push_branch(asm_branch);
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

    std::shared_ptr<FuncArgumentsBranch> func_arguments = std::shared_ptr<FuncArgumentsBranch>(new FuncArgumentsBranch(getCompiler()));
    // Set the root and scopes for the function arguments since we are about to overwrite the scope with the function argument scope
    setRootAndScopes(func_arguments);

    this->root_scope = func_arguments;
    start_local_scope(func_arguments);
    // Process all the function parameters
    while (true)
    {
        /* If the next token is a keyword then process a variable declaration*/
        peek();
        if (is_peek_type("keyword"))
        {
            process_variable_declaration();

            // Pop the resulting variable declaration and put it in the function arguments branch
            pop_branch();
            func_arguments->addChild(this->branch);
        }
        else
        {
            /* Its not a keyword so check for a right bracket or a comma*/
            if (is_peek_symbol(")"))
            {
                // Looks like we are done here so shift and pop the bracket from the stack then break
                shift_pop();
                break;
            }
            else if (is_peek_symbol(","))
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

    std::shared_ptr<FuncDefBranch> func_dec_branch = NULL;

    peek();
    if (is_peek_symbol(";"))
    {
        // This is a function definition not a declaration.
        func_dec_branch = std::shared_ptr<FuncDefBranch>(new FuncDefBranch(this->compiler));
        process_semicolon();
    }
    else
    {
        // This is a function declaration it has a body, it is declared.
        func_dec_branch = std::shared_ptr<FuncBranch>(new FuncBranch(this->compiler));
        std::shared_ptr<FuncBranch> func_branch = std::dynamic_pointer_cast<FuncBranch>(func_dec_branch);
        std::shared_ptr<BODYBranch> body_root = std::shared_ptr<BODYBranch>(new BODYBranch(compiler));
        this->current_function = func_branch;
        this->root_scope = body_root;
        // Process the function body
        process_body(body_root);
        this->current_function = NULL;

        // Pop off the body its no longer needed as we are already aware of
        pop_branch();
        func_branch->setBodyBranch(body_root);

    }

    func_dec_branch->setReturnTypeBranch(func_return_type);
    func_dec_branch->setNameBranch(func_name);
    func_dec_branch->setArgumentsBranch(func_arguments);

    // Finish the local scope for the function arguments.
    finish_local_scope();

    // Now push it back to the stack
    push_branch(func_dec_branch);
}

void Parser::process_body(std::shared_ptr<BODYBranch> body_root, bool new_scope)
{
    // Check that the next branch is a left bracket all bodys must start with them.
    shift_pop();
    if (!is_branch_symbol("{"))
    {
        error_expecting("{", this->branch_value);
    }

    // Sometimes the caller may wish to use their own body, and sometimes not.
    if (body_root == NULL)
    {
        body_root = std::shared_ptr<BODYBranch>(new BODYBranch(compiler));
    }

    // We need to set the scopes for this body manually since we are about to lose our scope
    setRootAndScopes(body_root);

    if (new_scope)
    {
        // Start the local scope.
        start_local_scope(body_root);
    }
    while (true)
    {
        // Check to see if we are at the end of the body
        peek();
        if (is_peek_symbol("}"))
        {
            // Shift and pop the right bracket
            shift_pop();

            // We are done.
            break;
        }
        else
        {
            if (this->current_function == NULL)
            {
                // We are in the global scope so process the top
                process_top();
            }
            else
            {
                // We are in a function so process the statement
                process_stmt();
            }

            // Pop off the result and store it in the body_root
            pop_branch();
            body_root->addChild(this->branch);
        }
    }


    if (new_scope)
    {
        // Finish the local scope
        finish_local_scope();
        body_root->setLocalScope(this->current_local_scope);
    }
    // Push the body root onto the branch stack
    push_branch(body_root, false);
}

// All possible body statements

void Parser::process_stmt()
{
    peek();
    if (is_peek_type("keyword"))
    {
        // Check to see if this is an "if" statement
        if (is_peek_value("if"))
        {
            // Process the "if" statement
            process_if_stmt();
        }
        else if (is_peek_value("__asm"))
        {
            process_inline_asm();
            process_semicolon();
        }
        else if (is_peek_value("struct"))
        {
            // Its a structure variable declaration
            process_structure_declaration();
            process_semicolon();
        }
        else if (is_peek_value("while"))
        {
            // This is a "while" statement so process it
            process_while_stmt();
        }
        else if (is_peek_value("for"))
        {
            // This is a "for" statement so process it
            process_for_stmt();
        }
        else if (is_peek_value("return"))
        {
            // This is a "return" statement so process it
            process_return_stmt();
            process_semicolon();
        }
        else if (is_peek_value("break"))
        {
            // This is a "break" statement so process it
            process_break();
            process_semicolon();
        }
        else if (is_peek_value("continue"))
        {
            // This is a "continue" statement so process it
            process_continue();
            process_semicolon();

        }
        else
        {
            // This is a variable declaration so process it
            process_variable_declaration();
            process_semicolon();
        }
    }
    else if (is_peek_type("identifier"))
    {
        peek(1);
        if (is_peek_symbol("("))
        {
            // A left bracket was found so it must be a function call
            process_function_call();
            process_semicolon();
        }
        else
        {
            // Ok it may be a variable assignment so lets just process the expression
            process_expression();
            process_semicolon();
        }
    }
    else if (is_peek_operator("*"))
    {
        // This is probably a pointer assignment so lets process the pointer
        process_expression();
        process_semicolon();
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
    std::shared_ptr<Branch> identifier_branch = NULL;
    std::shared_ptr<Branch> var_keyword_branch = NULL;
    std::shared_ptr<Branch> var_value_branch = NULL;

    std::shared_ptr<VDEFBranch> var_root = std::shared_ptr<VDEFBranch>(new VDEFBranch(this->getCompiler()));

    // Shift the keyword of the variable onto the stack
    shift_pop();
    if (!is_branch_type("keyword"))
    {
        error_expecting("keyword", this->branch_value);
    }

    // Check that the keyword is a data type
    if (!Lexer::isDataTypeKeyword(this->branch_value))
    {
        error("Expecting a data type keyword for a variable declaration");
    }

    var_keyword_branch = this->branch;

    // Lets see if we are defining a pointer
    peek();
    if (is_peek_operator("*"))
    {
        // Shift and pop the pointer operator
        shift_pop();

        // Lets find out how much pointer depth we have here
        int depth = 1 + get_pointer_depth();
        var_root->setPointer(true, depth);
    }

    // Process the variable access
    process_variable_access();
    pop_branch();

    identifier_branch = this->branch;

    peek();
    // Check if this is just a variable declaration or that we are also setting it to a value    
    if (is_peek_operator("="))
    {
        // Their is an equal sign so we are setting it to something
        // Shift and pop that "=" sign we do not need it anymore
        shift_pop();

        // Process the expression we are setting this variable too
        process_expression();
        // Pop the expression from the stack
        pop_branch();
        var_value_branch = this->branch;
    }



    var_root->setDataTypeBranch(var_keyword_branch);
    var_root->setVariableIdentifierBranch(identifier_branch);
    var_root->setValueExpBranch(var_value_branch);

    // Push that root back to the branches
    push_branch(var_root);

}

void Parser::process_ptr()
{
    shift_pop();
    if (!is_branch_operator("*"))
    {
        error_expecting("operator: *", this->branch_type);
    }

    int depth = 1 + get_pointer_depth();

    std::shared_ptr<PTRBranch> ptr_branch = std::shared_ptr<PTRBranch>(new PTRBranch(compiler));
    ptr_branch->setPointerDepth(depth);

    peek();
    if (is_peek_symbol("("))
    {
        // Process the expression
        process_expression();
        pop_branch();
    }
    else
    {
        // No expression? This is variable access.
        process_variable_access();
        pop_branch();
    }

    ptr_branch->setExpressionBranch(this->branch);

    // Push the pointer branch to the stack
    push_branch(ptr_branch);
}

void Parser::process_assignment(std::shared_ptr<Branch> left, std::shared_ptr<Branch> right, std::shared_ptr<Branch> op)
{
    // Check for a valid assignment, e.g =, +=, -=
    if (!is_assignment_operator(op->getValue()))
    {
        error("expecting one of the following operators for assignments: =,+=,-=,*=,/= but " + this->branch_value + " was provided.");
    }


    std::shared_ptr<AssignBranch> assign_branch = std::shared_ptr<AssignBranch>(new AssignBranch(this->getCompiler()));
    assign_branch->setValue(op->getValue());
    assign_branch->setVariableToAssignBranch(left);
    assign_branch->setValueBranch(right);

    // Now finally push the assign branch to the stack
    push_branch(assign_branch);
}

void Parser::process_variable_access(std::shared_ptr<STRUCTDEFBranch> last_struct_def)
{
    std::shared_ptr<VarIdentifierBranch> var_identifier_branch = std::shared_ptr<VarIdentifierBranch>(new VarIdentifierBranch(compiler));
    peek();
    if (is_peek_type("identifier"))
    {
        shift_pop();
        var_identifier_branch->setVariableNameBranch(this->branch);
    }
    else
    {
        error_expecting("identifier", this->token_type);
    }

    // peek ahead further to check if their is array access
    peek();
    if (is_peek_symbol("["))
    {
        // We have array access process it
        process_array_indexes();
        pop_branch();
        var_identifier_branch->setRootArrayIndexBranch(this->branch);
    }

    // peek further to see if there is any structure access
    peek();
    if (is_peek_symbol("."))
    {
        process_structure_access();
        pop_branch();
        var_identifier_branch->setStructureAccessBranch(this->branch);
    }

    push_branch(var_identifier_branch);
}

void Parser::process_structure_descriptor()
{
    peek();
    if (!is_peek_keyword("struct"))
    {
        error_expecting("struct", this->peek_token_value);
    }

    // Shift and pop the "struct" keyword we don't care about it now
    shift_pop();

    peek();
    if (!is_peek_type("identifier"))
    {
        error_expecting("identifier", this->peek_token_type);
    }

    // Lets get the name of this structure
    shift_pop();
    std::shared_ptr<Branch> struct_name_branch = this->branch;

    std::shared_ptr<STRUCTDescriptorBranch> struct_desc_branch 
            = std::shared_ptr<STRUCTDescriptorBranch>(new STRUCTDescriptorBranch(getCompiler()));
    struct_desc_branch->setStructNameBranch(struct_name_branch);

    push_branch(struct_desc_branch);
}

void Parser::process_structure_access()
{
    std::shared_ptr<STRUCTAccessBranch> struct_access_root = NULL;

    // Check to see if this structure access is valid
    peek();
    if (!is_peek_symbol("."))
    {
        error_expecting(".", this->peek_token_value);
    }

    // Shift and pop off the "."
    shift_pop();

    // Process the variable identifier
    process_variable_access();
    pop_branch();

    struct_access_root = std::shared_ptr<STRUCTAccessBranch>(new STRUCTAccessBranch(compiler));
    struct_access_root->setVarIdentifierBranch(std::dynamic_pointer_cast<VarIdentifierBranch>(this->branch));
    push_branch(struct_access_root);
}

void Parser::process_expression(PARSER_EXPRESSION_OPTIONS options)
{
    std::shared_ptr<Branch> last = NULL;
    std::shared_ptr<Branch> tmp = NULL;
    do
    {
        // Process the expression part, e.g "5 + 5"
        process_expression_part(last, options);
        // Pop off the result
        pop_branch();
        last = this->branch;

        // Peek ahead to see if order of operations applies
        peek();
        if (is_peek_type("operator"))
        {
            ORDER_OF_OPERATIONS_PRIORITY priority = get_order_of_operations_priority(last->getValue(), this->peek_token_value);
            if (priority == ORDER_OF_OPERATIONS_RIGHT_GREATER)
            {
                // Ok order of operations applies here so we need to process it and replace the right branch of the last expression part
                process_expression_part(last->getSecondChild()->clone(), options);
                // Pop off the result
                pop_branch();
                tmp = this->branch;
                // Replace the branch with the new branch
                last->getSecondChild()->replaceSelf(tmp);
            }
        }

        // Peek ahead to see if we are done or not
        peek();
    }
    while (is_peek_type("operator") && !compiler->isLogicalOperator(this->peek_token_value));

    std::shared_ptr<Branch> exp_root = last;
    push_branch(exp_root);

    peek();
    // Do we have a logical operator if so then we have more to do
    if (compiler->isLogicalOperator(this->peek_token_value))
    {
        // Ok we do shift and pop it off
        shift_pop();
        std::shared_ptr<Branch> op = this->branch;

        // Pop off the previous expression from the stack
        pop_branch();
        std::shared_ptr<Branch> left = this->branch;

        // Now process the new expression
        process_expression(options);

        // Pop it off
        pop_branch();
        std::shared_ptr<Branch> right = this->branch;

        // Put it all together
        std::shared_ptr<Branch> exp = std::shared_ptr<EBranch>(new EBranch(this->compiler, op->getValue()));
        exp->addChild(left);
        exp->addChild(right);
        push_branch(exp);
    }

}

void Parser::process_expression_part(std::shared_ptr<Branch> left, PARSER_EXPRESSION_OPTIONS options)
{
    std::shared_ptr<Branch> exp_root = NULL;
    std::shared_ptr<Branch> op = NULL;
    std::shared_ptr<Branch> right = NULL;

    // Do we need to process a left operand?
    if (left == NULL)
    {
        // Process the left operand
        left = process_expression_operand(options);
    }

    // Do we have an operator?
    peek();
    if (is_peek_type("operator"))
    {
        // Yes we do so pop it off
        shift_pop();
        op = this->branch;

        // Process the right operand
        right = process_expression_operand(options);
    }

    if (left != NULL && op != NULL && right != NULL)
    {
        if (is_assignment_operator(op->getValue()))
        {
            // This is an assignment so lets work with it a bit differently
            process_assignment(left, right, op);
            pop_branch();
            exp_root = this->branch;
        }
        else
        {
            // Ok all left op and right are present so lets create an expression root branch
            exp_root = std::shared_ptr<EBranch>(new EBranch(getCompiler(), op->getValue()));
            exp_root->addChild(left);
            exp_root->addChild(right);
        }
    }
    else
    {
        // Ok only left available, set it as the expression root
        exp_root = left;
    }

    push_branch(exp_root);
}

std::shared_ptr<Branch> Parser::process_expression_operand(PARSER_EXPRESSION_OPTIONS options)
{
    peek();
    std::shared_ptr<Branch> b = NULL;
    if (is_peek_symbol("("))
    {
        // Looks like we have a bracket here, lets deal with it
        shift_pop();
        // Brackets mean expressions, process the expression
        process_expression(options);
        // Pop off the expression
        pop_branch();
        b = this->branch;

        // expressions that start with a bracket must end with an ending bracket
        peek();
        if (!is_peek_symbol(")"))
        {
            error_expecting(")", this->peek_token_value);
        }

        // Pop off the right bracket as we don't need it anymore
        shift_pop();
    }
    else if (is_peek_type("number"))
    {
        // Shift and pop the number
        shift_pop();
        b = this->branch;
    }
    else if (is_peek_type("identifier"))
    {
        peek(1);
        // Their is a left bracket so this must be a function call
        if (is_peek_symbol("("))
        {
            process_function_call();
            // Pop the result from the stack
            pop_branch();
            b = this->branch;
        }
        else
        {
            if (options & PARSER_EXPRESSION_USE_IDENTIFIER_INSTEAD_OF_VAR_IDENTIFIER)
            {
                // Options tell us to just treat this as an identifier, not variable access
                process_identifier();
                pop_branch();
                b = this->branch;
            }
            else
            {
                // Process the variable access
                process_variable_access();
                // Pop off the result
                pop_branch();
                b = this->branch;
            }
        }
    }
    else if (is_peek_operator("&"))
    {
        // We are getting the address of a declaration here
        // Shift and pop the "&" symbol we do not need it anymore
        shift_pop();

        // This is an identifier, e.g a variable name so process the variable access
        process_variable_access();
        pop_branch();

        std::shared_ptr<AddressOfBranch> address_of_branch = std::shared_ptr<AddressOfBranch>(new AddressOfBranch(this->getCompiler()));
        address_of_branch->setVariableBranch(this->branch);
        b = address_of_branch;
    }
    else if (is_peek_operator("*"))
    {
        process_ptr();
        pop_branch();
        b = this->branch;
    }
    else if (is_peek_operator("!"))
    {
        // This is a logical NOT statement.
        process_logical_not();
        pop_branch();
        b = this->branch;
    }
    else if (is_peek_operator("-"))
    {
        // This is a negative number, pop off the negative operator
        shift_pop();
        // Shift and pop the number
        shift_pop();
        b = this->branch;
        // Make it negative
        b->setValue("-" + b->getValue());
    }
    else if (is_peek_type("string"))
    {
        // We have a string shift and pop the string 

        shift_pop();
        b = this->branch;
    }
    else if (is_peek_symbol("#"))
    {
        process_macro();
        pop_branch();

        b = this->branch;
    }

    return b;
}

std::shared_ptr<Branch> Parser::process_expression_operator()
{
    shift_pop();
    if (!is_branch_type("operator"))
    {

        error("expecting operator");
    }
    return this->branch;
}

void Parser::process_function_call(std::shared_ptr<Branch>* func_name_branch, std::shared_ptr<Branch>* func_params_branch, std::function<void() > handle_func_param)
{
    shift_pop();
    // Check that the branch is an identifier as function calls require them
    if (!is_branch_type("identifier"))
    {
        error("missing identifier for function call");
    }

    std::shared_ptr<Branch> func_name = this->branch;

    shift_pop();
    // Check that the branch is a left bracket as a function call would require one at this stage
    if (!is_branch_symbol("("))
    {
        error("missing left bracket for function call");
    }


    std::shared_ptr<Branch> params = std::shared_ptr<Branch>(new Branch("PARAMS", ""));
    // So far so good now we need to get the function call parameters
    while (true)
    {
        peek();
        if (is_peek_symbol(")"))
        {
            // Right curly was found so we are done
            // Pop it off then break
            shift_pop();
            break;
        }
        else if (is_peek_symbol(","))
        {
            // Their is a comma so just pop it off
            shift_pop();
        }
        else
        {
            // Invoke the handle function parameter function passed to us
            handle_func_param();
            // Pop the resulting expression
            pop_branch();
            // Add it to the params
            params->addChild(this->branch);
        }
    }

    *func_name_branch = func_name;
    *func_params_branch = params;

}

void Parser::process_function_call()
{
    std::shared_ptr<Branch> func_name;
    std::shared_ptr<Branch> func_params;

    // Process the function call
    process_function_call(&func_name, &func_params, [&]
    {
        /* This is called every time we are at a function parameter on the stack e.g test(HERE, HERE, HERE) 
         * so we are just going to process the expression of this function parameter*/
        process_expression();
    });

    // We have everything we need now build the function call
    std::shared_ptr<FuncCallBranch> func_call_root = std::shared_ptr<FuncCallBranch>(new FuncCallBranch(this->getCompiler()));
    func_call_root->setFuncNameBranch(func_name);
    func_call_root->setFuncParamsBranch(func_params);
    push_branch(func_call_root);
}

void Parser::process_if_stmt()
{
    // Check to see if the next token is an identifier and its value is "if"
    shift_pop();
    if (!is_branch_keyword("if"))
    {
        error("expecting an identifier of value \"if\" for an if statement");
    }

    // Check to see that the next token is a left bracket as "if" statements require them
    shift_pop();
    if (!is_branch_symbol("("))
    {
        error_expecting("(", this->branch_value);
    }

    // Process the "if" statement expression
    process_expression();

    // Pop off the result expression
    pop_branch();
    std::shared_ptr<Branch> if_exp = this->branch;

    // Shift and pop off the right bracket
    shift_pop();
    if (!is_branch_symbol(")"))
    {
        // Its not a right bracket so complain..
        error_expecting(")", this->branch_value);
    }

    // Process the body
    process_body();
    // Pop off the body
    pop_branch();
    std::shared_ptr<BODYBranch> if_body = std::dynamic_pointer_cast<BODYBranch>(this->branch);

    std::shared_ptr<IFBranch> if_stmt = std::shared_ptr<IFBranch>(new IFBranch(this->getCompiler()));
    if_stmt->setExpressionBranch(if_exp);
    if_stmt->setBodyBranch(if_body);

    // Check for an else statement
    peek();
    if (is_peek_keyword("else"))
    {
        // Shift and pop the else keyword we do not need it anymore
        shift_pop();

        // peek to see if its an else if statement
        peek();
        if (is_peek_keyword("if"))
        {
            // No need to shift and pop the "if" keyword as the "process_if_stmt()" method requires it present
            process_if_stmt();
            // Pop off the if statement
            pop_branch();

            // Add the else if branch to this if statement branch.
            if_stmt->setElseIfBranch(std::dynamic_pointer_cast<IFBranch>(this->branch));
        }
        else
        {

            std::shared_ptr<ELSEBranch> else_stmt = std::shared_ptr<ELSEBranch>(new ELSEBranch(this->getCompiler()));
            // Process the body of the else statement
            process_body();
            // Pop the result off the stack
            pop_branch();
            // Add the body to the else statement
            else_stmt->setBodyBranch(std::dynamic_pointer_cast<BODYBranch>(this->branch));
            // Add the else statement to the if statement
            if_stmt->setElseBranch(else_stmt);
        }
    }
    // Push the complete "if" statement to the tree
    push_branch(if_stmt);
}

void Parser::process_return_stmt()
{
    // Check that the return keyword is present
    shift_pop();
    if (!is_branch_keyword("return"))
    {
        error_expecting("return", this->branch_value);
    }

    std::shared_ptr<Branch> exp = NULL;
    // peek ahead do we have a semicolon? if so we are done otherwise their is an expression that is being returned
    peek();
    if (!is_peek_symbol(";"))
    {
        // We do not have a semicolon so their is an expression to parse
        process_expression();
        // Pop off the result
        pop_branch();
        exp = this->branch;
    }

    // Create the return branch
    std::shared_ptr<ReturnBranch> return_branch = std::shared_ptr<ReturnBranch>(new ReturnBranch(this->compiler));
    // If their was an expression then we need to add it to the return branch
    if (exp != NULL)
    {
        return_branch->setExpressionBranch(exp);
    }

    // Finally push the return branch to the stack
    push_branch(return_branch);
}

void Parser::process_structure()
{
    // Shift and pop off the "struct" keyword we do not need it anymore
    shift_pop();
    // Check that it is actually a "struct" keyword
    if (!is_branch_keyword("struct"))
    {
        error("Expecting \"struct\" keyword but token: " + this->branch_value + " was given");
    }

    // Shift and pop off the name of the structure and check that it is an identifier
    shift_pop();
    if (!is_branch_type("identifier"))
    {

        error("Expecting identifier for \"struct\" name but token type: "
              + this->branch_type + " of value: " + this->branch_value + " was provided");
    }

    std::shared_ptr<Branch> struct_name = this->branch;

    // Process the body of the structure
    process_body();

    // Pop off the resulting body
    pop_branch();
    std::shared_ptr<BODYBranch> struct_body = std::dynamic_pointer_cast<BODYBranch>(this->branch);

    // Create the structure branch
    std::shared_ptr<STRUCTBranch> struct_root = std::shared_ptr<STRUCTBranch>(new STRUCTBranch(compiler));
    // Add the structure name to the structure
    struct_root->setStructNameBranch(struct_name);
    // Add the body to the structure
    struct_root->setStructBodyBranch(struct_body);

    // Finally add the structure root to the main tree
    push_branch(struct_root);

    // Lets just add the declared structure to the vector so we can access it later during parsing
    this->declared_structs.push_back(struct_root);
}

void Parser::process_structure_declaration()
{
    std::shared_ptr<STRUCTDEFBranch> struct_declaration = std::shared_ptr<STRUCTDEFBranch>(new STRUCTDEFBranch(compiler));
    // Shift and pop the token and check that it is a "keyword" equal to "struct"
    shift_pop();
    if (!is_branch_keyword("struct"))
    {
        error_expecting("struct", this->token_value);
    }


    // Shift and pop the token and check that it is an "identifier" this is the structure name
    shift_pop();
    if (!is_branch_type("identifier"))
    {
        error_expecting("identifier", this->token_value);
    }

    std::shared_ptr<Branch> struct_name_branch = this->branch;

    // Lets check to see if this is a pointer declaration
    peek();
    if (is_peek_operator("*"))
    {
        // Shift and pop the operator from the stack as we do not need it anymore
        shift_pop();
        // Lets find out how much depth this pointer is
        int depth = 1 + get_pointer_depth();
        struct_declaration->setPointer(true, depth);
    }

    // process the variable access
    process_variable_access();
    pop_branch();

    std::shared_ptr<Branch> identifier_branch = this->branch;

    std::shared_ptr<Branch> var_value_branch = NULL;
    // peek ahead to see if their is an = sign it may be a structure declaration
    peek();
    if (is_peek_operator("="))
    {
        // Ok their is an equal sign so we must be assigning this declaration
        // Shift and pop the equal sign we no longer need it
        shift_pop();

        // peek further to check if we have an identifier
        peek();
        if (is_peek_type("identifier"))
        {
            // Shift and pop the identifier from the stack
            shift_pop();
            var_value_branch = this->branch;
        }
        else
        {

            error_expecting("identifier", this->peek_token_value);
        }
    }

    struct_declaration->setDataTypeBranch(struct_name_branch);
    struct_declaration->setVariableIdentifierBranch(identifier_branch);
    struct_declaration->setValueExpBranch(var_value_branch);

    // Now push it to the stack
    push_branch(struct_declaration);

}

void Parser::process_while_stmt()
{
    // shift and pop the next token and make sure its a "while" keyword
    shift_pop();
    if (!is_branch_keyword("while"))
    {
        error_expecting("while", this->branch_value);
    }

    // shift and pop the next token and make sure its a left bracket.
    shift_pop();
    if (!is_branch_symbol("("))
    {
        error_expecting("(", this->branch_value);
    }

    // Process the expression
    process_expression();
    // Pop off the expression result
    pop_branch();
    std::shared_ptr<Branch> exp = this->branch;

    // Shift and pop the right bracket and make sure it is a right bracket
    shift_pop();
    if (!is_branch_symbol(")"))
    {

        error_expecting(")", this->branch_value);
    }

    // Process the body
    process_body();
    // Pop off the body result
    pop_branch();
    std::shared_ptr<BODYBranch> body = std::dynamic_pointer_cast<BODYBranch>(this->branch);

    // Time to put it all together
    std::shared_ptr<WhileBranch> while_stmt = std::shared_ptr<WhileBranch>(new WhileBranch(getCompiler()));
    while_stmt->setExpressionBranch(exp);
    while_stmt->setBodyBranch(body);

    // Finally push the while statement to the tree
    push_branch(while_stmt);
}

void Parser::process_for_stmt()
{
    std::shared_ptr<FORBranch> for_stmt = std::shared_ptr<FORBranch>(new FORBranch(compiler));

    std::shared_ptr<Branch> init_var = NULL;
    std::shared_ptr<Branch> cond_exp = NULL;
    std::shared_ptr<Branch> loop_stmt = NULL;
    std::shared_ptr<BODYBranch> body = NULL;

    // Handle the init part of the statement

    // Shift and pop the next token and check its a "for" keyword
    shift_pop();
    if (!is_branch_keyword("for"))
    {
        error_expecting("for", this->branch_value);
    }

    // Shift and pop the next token it should be a left bracket
    shift_pop();
    if (!is_branch_symbol("("))
    {
        error_expecting("(", this->branch_value);
    }

    /* Now we are either expecting a variable declaration, assignment or both.
     * We also allow just a semicolon if the programmer does not wish to define or set anything*/
    peek();

    // Start the "for_stmt" local scope.
    start_local_scope(for_stmt);
    if (is_peek_type("keyword") || is_peek_type("identifier"))
    {
        if (is_peek_type("keyword"))
        {
            // Ok their is a variable declaration here
            // Process the variable declaration
            process_variable_declaration();
            // Pop off the result
            pop_branch();
        }
        else
        {
            // Ok its a variable assignment
            // Process the variable assignment by calling "process_expression"
            process_expression();
            // Pop off the result
            pop_branch();
        }

        // Store the result of the variable declaration, assignment or both in the init_var branch
        init_var = this->branch;
    }

    // Process and pop off the semicolon
    process_semicolon();

    // Process the condition if any.
    peek();
    if (!is_peek_symbol(";"))
    {
        // Now process the condition
        process_expression();
        // Pop off the condition expression
        pop_branch();
        cond_exp = this->branch;
    }

    // Process and pop off the semicolon
    process_semicolon();

    // Process the loop part of the "for" statement, if any
    peek();
    if (!is_peek_symbol(")"))
    {
        // Process the assignment by calling the "process_expression" method.
        process_expression();

        // Pop off the result and store it in the "loop_stmt" variable
        pop_branch();
        loop_stmt = this->branch;
    }

    // Shift and pop off the right bracket we do not need it anymore
    shift_pop();
    // Check that it was actually a right bracket
    if (!is_branch_symbol(")"))
    {

        error_expecting(")", this->branch_value);
    }

    // Process the "for" loop body
    process_body();

    // Finish the scope for the "for_stmt"
    finish_local_scope();

    // Pop the resulting body from the stack
    pop_branch();

    body = std::dynamic_pointer_cast<BODYBranch>(this->branch);
    // Put it all together
    for_stmt->setInitBranch(init_var);
    for_stmt->setCondBranch(cond_exp);
    for_stmt->setLoopBranch(loop_stmt);
    for_stmt->setBodyBranch(body);

    // Push the for statement to the stack
    push_branch(for_stmt);
}

void Parser::process_array_indexes()
{
    // Shift and pop the left bracket and check it is a left bracket
    shift_pop();
    if (!is_branch_symbol("["))
    {
        error_expecting("[", this->token_value);
    }

    // Process the expression e.g [(exp here)]
    process_expression();
    pop_branch();

    std::shared_ptr<Branch> expression = this->branch;

    // Shift and pop the right bracket and check that it is a right bracket
    shift_pop();
    if (!is_branch_symbol("]"))
    {
        error_expecting("]", this->token_value);
    }

    std::shared_ptr<ArrayIndexBranch> array_index_branch = std::shared_ptr<ArrayIndexBranch>(new ArrayIndexBranch(compiler));
    array_index_branch->setValueBranch(expression);
    // Check to see if the next token is a left bracket if it is we are not done
    peek();
    if (is_peek_symbol("["))
    {
        // Yes there are more indexes to go so recall ourself

        process_array_indexes();
        pop_branch();
        // Pop the result and attach it as a child to our array index branch
        array_index_branch->setNextArrayIndexBranch(std::dynamic_pointer_cast<ArrayIndexBranch>(this->branch));
    }

    // Push our resulting array index branch
    push_branch(array_index_branch);

}

void Parser::process_semicolon()
{
    // Shift and pop the semicolon off the stack.
    shift_pop();

    // Check that it was a semicolon
    if (!is_branch_symbol(";"))
    {

        error("expecting a semicolon, however token: \"" + this->token_value + "\" was provided");
    }
}

void Parser::process_identifier()
{
    peek();
    if (!is_peek_type("identifier"))
    {

        error("expecting an identifier, however token: \"" + this->token_value + "\" was provided");
    }

    // Shift the identifier to the stack
    shift();
}

void Parser::process_logical_not()
{
    shift_pop();
    if (!is_branch_operator("!"))
    {
        error_expecting("!", this->branch_type);
    }

    std::shared_ptr<LogicalNotBranch> logical_not_branch = std::shared_ptr<LogicalNotBranch>(new LogicalNotBranch(this->compiler));
    peek();
    if (is_peek_symbol("("))
    {
        // Process the expression
        process_expression();
        pop_branch();
    }
    else
    {
        // No expression? This is variable access.
        process_variable_access();
        pop_branch();
    }

    logical_not_branch->setSubjectBranch(this->branch);
    push_branch(logical_not_branch);
}

void Parser::process_break()
{
    shift_pop();
    if (!is_branch_keyword("break"))
    {
        error_expecting("break", this->branch_value);
    }

    std::shared_ptr<BreakBranch> break_branch = std::shared_ptr<BreakBranch>(new BreakBranch(getCompiler()));
    push_branch(break_branch);
}

void Parser::process_continue()
{
    shift_pop();
    if (!is_branch_keyword("continue"))
    {
        error_expecting("continue", this->branch_value);
    }

    std::shared_ptr<ContinueBranch> continue_branch = std::shared_ptr<ContinueBranch>(new ContinueBranch(getCompiler()));
    push_branch(continue_branch);
}

void Parser::process_macro_ifdef()
{
    shift_pop();
    if (!is_branch_keyword("ifdef"))
    {
        error_expecting("ifdef", this->branch_value);
    }

    // Lets get the requirement
    shift_pop();
    if (!is_branch_type("identifier"))
    {
        error_expecting("identifier", this->branch_type);
    }

    std::shared_ptr<Branch> requirement_branch = this->branch;

    // Ok lets get the body, we don't want to create a new scope as macros are interpreted not compiled.
    process_body(NULL, false);
    pop_branch();

    std::shared_ptr<BODYBranch> body_branch = std::dynamic_pointer_cast<BODYBranch>(this->branch);

    // Ok lets put it all together

    std::shared_ptr<MacroIfDefBranch> macro_if_def_branch = std::shared_ptr<MacroIfDefBranch>(new MacroIfDefBranch(getCompiler()));
    macro_if_def_branch->setRequirementBranch(requirement_branch);
    macro_if_def_branch->setBodyBranch(body_branch);

    // Ok we are done push it to the stack
    push_branch(macro_if_def_branch);

}

void Parser::process_macro_define()
{
    shift_pop();
    if (!is_branch_keyword("define"))
    {
        error_expecting("define", this->branch_value);
    }

    // Process the definition name
    process_identifier();
    pop_branch();
    std::shared_ptr<Branch> def_name = this->branch;

    std::shared_ptr<MacroDefineBranch> macro_define_branch = std::shared_ptr<MacroDefineBranch>(new MacroDefineBranch(getCompiler()));
    macro_define_branch->setDefinitionNameBranch(def_name);

    peek();
    // Do we have a value for this definition?
    if (is_peek_type("identifier") || is_peek_type("number") || is_peek_type("string"))
    {
        // Process the definition value expression
        process_expression(PARSER_EXPRESSION_USE_IDENTIFIER_INSTEAD_OF_VAR_IDENTIFIER);
        pop_branch();
        std::shared_ptr<Branch> value_exp = this->branch;
        macro_define_branch->setDefinitionValueBranch(value_exp);
    }

    // Ok finally lets push this to the stack
    push_branch(macro_define_branch);

}

void Parser::process_macro_function_call()
{
    std::shared_ptr<Branch> func_name;
    std::shared_ptr<Branch> func_params;

    // Process the macro function call
    process_function_call(&func_name, &func_params, [&]
    {
        // We need to process the function parameter here, as this is a macro function call it can be a keyword, a structure, a expression, or anything a normal function call can do

        // Lets first check for a keyword
        peek();
        if (is_peek_type("keyword"))
        {
                          if (is_peek_keyword("struct") &&
                          is_peek_type("identifier", 1) &&
                          !is_peek_type("identifier", 2))
            {
                          // Ok this is just showing a structure descriptor not a structure definition, e.g its showing "struct test" not "struct test a"
                          process_structure_descriptor();
                          return;
            }
            else if (is_peek_type("symbol", 1))
            {
                          // Ok this is just a keyword on its own lets just shift it onto the stack
                          shift();
                          return;
            }
        }
        
        // Ok nothing has been done so lets just treat it as an expression
        process_expression();

    });

    std::shared_ptr<MacroFuncCallBranch> macro_func_call_branch
            = std::shared_ptr<MacroFuncCallBranch>(new MacroFuncCallBranch(this->compiler));

    macro_func_call_branch->setFuncNameBranch(func_name);
    macro_func_call_branch->setFuncParamsBranch(func_params);

    // Lets push the result to the stack
    push_branch(macro_func_call_branch);

}

void Parser::process_macro_definition_identifier()
{
    process_identifier();
    pop_branch();

    std::shared_ptr<Branch> iden_branch = this->branch;

    // Now lets construct the macro definition identifier branch
    std::shared_ptr<MacroDefinitionIdentifierBranch> macro_def_iden_branch
            = std::shared_ptr<MacroDefinitionIdentifierBranch>(new MacroDefinitionIdentifierBranch(getCompiler()));

    macro_def_iden_branch->setIdentifierBranch(iden_branch);

    push_branch(macro_def_iden_branch);

}

void Parser::error(std::string message, bool token)
{
    if (token && this->token != NULL)
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

void Parser::peek(int offset)
{
    if (!this->input.empty())
    {
        if (offset == -1)
        {
            this->peek_token = this->input.front();
        }
        else
        {
            if (offset < this->input.size())
            {
                this->peek_token = this->input.at(offset);
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
    error("peek failed, no more input with unfinished parse, check your source file.", false);
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

void Parser::setRootAndScopes(std::shared_ptr<Branch> branch)
{
    branch->setRoot(this->root_branch);
    // We should not set scopes to themselves!
    if (this->current_local_scope != branch)
        branch->setLocalScope(this->current_local_scope);
    if (this->root_scope != branch)
        branch->setRootScope(this->root_scope);
}

void Parser::push_branch(std::shared_ptr<Branch> branch, bool apply_scopes_to_branch)
{
    if (branch == NULL)
    {
        throw Exception("void Parser::push_branch(std::shared_ptr<Branch> branch): branch may not be NULL");
    }
    if (apply_scopes_to_branch)
    {
        // Before pushing we must assign the root branch and the scopes to the branch
        setRootAndScopes(branch);
    }
    this->branches.push_back(branch);
}

void Parser::shift_pop()
{
    this->shift();
    this->pop_branch();
}

void Parser::start_local_scope(std::shared_ptr<ScopeBranch> local_scope)
{
    this->current_local_scope = local_scope;
    this->local_scopes.push_back(local_scope);
}

void Parser::finish_local_scope()
{
    this->local_scopes.pop_back();
    if (!this->local_scopes.empty())
    {
        this->current_local_scope = this->local_scopes.back();
    }
}

void Parser::handle_left_or_right(std::shared_ptr<Branch>* left, std::shared_ptr<Branch>* right)
{
    if (*left == NULL)
    {
        *left = process_expression_operand();
    }
    else if (*right == NULL)
    {
        *right = process_expression_operand();
    }
}

bool Parser::is_branch_symbol(std::string symbol)
{

    return is_branch_type("symbol") && is_branch_value(symbol);
}

bool Parser::is_branch_type(std::string type)
{

    return this->branch_type == type;
}

bool Parser::is_branch_value(std::string value)
{

    return this->branch_value == value;
}

bool Parser::is_branch_keyword(std::string keyword)
{

    return is_branch_type("keyword") && is_branch_value(keyword);
}

bool Parser::is_branch_operator(std::string op)
{

    return is_branch_type("operator") && is_branch_value(op);
}

bool Parser::is_branch_identifier(std::string identifier)
{

    return is_branch_type("identifier") && is_branch_value(identifier);
}

bool Parser::is_peek_symbol(std::string symbol)
{

    return is_peek_type("symbol") && is_peek_value(symbol);
}

bool Parser::is_peek_symbol(std::string symbol, int peek)
{
    if (peek < this->input.size())
    {
        std::shared_ptr<Token> peek_token = this->input.at(peek);
        if (peek_token->getType() == "symbol" && peek_token->getValue() == symbol)
        {
            return true;
        }
    }
    else
    {

        error("bool Parser::is_peek_type(std::string type, int peek): peek offset is breaching bounds.");
    }

    return false;
}

bool Parser::is_peek_type(std::string type)
{

    return this->peek_token_type == type;
}

bool Parser::is_peek_type(std::string type, int peek)
{
    if (peek < this->input.size())
    {
        std::shared_ptr<Token> peek_token = this->input.at(peek);
        if (peek_token->getType() == type)
        {
            return true;
        }
    }
    else
    {

        error("bool Parser::is_peek_type(std::string type, int peek): peek offset is breaching bounds.");
    }

    return false;
}

bool Parser::is_peek_value(std::string value)
{

    return this->peek_token_value == value;
}

bool Parser::is_peek_keyword(std::string keyword)
{

    return is_peek_type("keyword") && is_peek_value(keyword);
}

bool Parser::is_peek_operator(std::string op)
{

    return is_peek_type("operator") && is_peek_value(op);
}

bool Parser::is_peek_operator(std::string op, int peek)
{
    if (peek < this->input.size())
    {
        std::shared_ptr<Token> peek_token = this->input.at(peek);
        if (peek_token->getType() == "operator" &&
                peek_token->getValue() == op)
        {
            return true;
        }
    }
    else
    {

        error("bool Parser::is_peek_operator(std::string op, int peek): peek offset is breaching bounds.");
    }

    return false;
}

bool Parser::is_peek_identifier(std::string identifier)
{
    return is_peek_type("identifier") && is_peek_value(identifier);
}

bool Parser::is_assignment_operator(std::string op)
{
    return getCompiler()->isAssignmentOperator(op);
}

int Parser::get_order_of_operations_priority_for_operator(std::string op)
{
    int size = sizeof (o_of_operation) / sizeof (struct order_of_operation);
    for (int i = 0; i < size; i++)
    {
        if (o_of_operation[i].op == op)
            return o_of_operation[i].priority;
    }

    // Operator not found in list so use priority zero
    return 0;
}

ORDER_OF_OPERATIONS_PRIORITY Parser::get_order_of_operations_priority(std::string lop, std::string rop)
{
    int l_priority = get_order_of_operations_priority_for_operator(lop);
    int r_priority = get_order_of_operations_priority_for_operator(rop);

    if (l_priority == r_priority)
        return ORDER_OF_OPERATIONS_PRIORITIES_EQUAL;
    else if (l_priority > r_priority)
        return ORDER_OF_OPERATIONS_LEFT_GREATER;
    else
        return ORDER_OF_OPERATIONS_RIGHT_GREATER;
}

std::shared_ptr<STRUCTBranch> Parser::getDeclaredStructure(std::string struct_name)
{
    for (std::shared_ptr<STRUCTBranch> struct_branch : this->declared_structs)
    {
        if (struct_branch->getStructNameBranch()->getValue() == struct_name)
        {
            return struct_branch;
        }
    }

    return NULL;
}

int Parser::get_pointer_depth()
{
    int depth = 0;
    // Lets keep checking for more "*" operators as this may not be a 1D pointer
    while (true)
    {
        peek();
        if (is_peek_operator("*"))
        {
            // Shift and pop the "*" operator
            shift_pop();
            // Increase the depth
            depth++;
        }
        else
        {
            break;
        }
    }
    return depth;
}

void Parser::buildTree()
{
    if (this->input.empty())
    {
        throw ParserException("Nothing to parse.");
    }

    this->root_branch = std::shared_ptr<RootBranch>(new RootBranch(this->compiler));
    while (!this->input.empty())
    {
        this->process_top();
    }

    while (!this->branches.empty())
    {
        std::shared_ptr<Branch> branch = this->branches.front();
        this->root_branch->addChild(branch);
        this->branches.pop_front();
    }
    this->tree->root = this->root_branch;

}

std::shared_ptr<Tree> Parser::getTree()
{
    return this->tree;
}

std::shared_ptr<Logger> Parser::getLogger()
{
    return this->logger;
}