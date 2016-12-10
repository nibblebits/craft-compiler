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
#include "Lexer.h"
#include "branches.h"
#include "common.h"

Parser::Parser(Compiler* compiler) : CompilerEntity(compiler)
{
    this->tree = std::shared_ptr<Tree>(new Tree());
    this->logger = std::shared_ptr<Logger>(new Logger());
    this->token = NULL;
    this->root_branch = NULL;
    this->root_scope = NULL;
    this->current_local_scope = NULL;
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
    peak();
    if (is_peak_type("keyword"))
    {
        std::string keyword_value = this->peak_token_value;
        // Is it a structure definition or declaration?
        if (keyword_value == "struct")
        {
            // Peak further to see if their is an identifier
            peak(1);
            if (is_peak_type("identifier"))
            {
                // Peak further again to see if their is yet another identifier
                peak(2);
                if (is_peak_type("identifier"))
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
            peak(1);
            if (is_peak_type("identifier"))
            {
                // Check to see if this is a function or a variable declaration
                peak(2);
                if (is_peak_symbol("[") || is_peak_operator("=") || is_peak_symbol(";"))
                {
                    process_variable_declaration();
                    process_semicolon();

                }
                else if (is_peak_symbol("("))
                {
                    // It is looking like a function call.
                    process_function();
                }
                else
                {
                    error_unexpected_token();
                }
            }
            else if (is_peak_operator("*"))
            {
                process_variable_declaration();
                process_semicolon();
            }
        }
    }
    else if (is_peak_symbol("#"))
    {
        // Its a macro so process it
        process_macro();
    }
    else
    {
        error_unexpected_token();
    }
}

void Parser::process_macro()
{
    // Check that the next token is a hash as all macros require them
    shift_pop();
    if (!is_branch_symbol("#"))
    {
        error_expecting("#", this->branch_value);
    }

    // Check that the next token is an identifier all macros will start with them
    shift_pop();
    if (!is_branch_type("identifier"))
    {
        error_expecting("identifier", this->branch_value);
    }

    if (is_branch_value("include"))
    {
        // This is a macro include we will be required to include a file.
        // Get the filename
        shift_pop();
        if (!is_branch_type("string"))
        {
            error_expecting("string", this->token_value);
        }

        std::string filename = this->branch->getValue();
        process_semicolon();

        /*
         *  We are now ready to load that file include, we will create a new parser to parse it 
         * then merge the result with our main tree */

        try
        {
            // Load the file
            std::string input = LoadFile(filename);
            // Perform lexical analysis on the input file
            Lexer lexer(this->getCompiler());
            lexer.setInput(input);
            lexer.tokenize();

            // Parse the token stream
            Parser sub_parser(this->getCompiler());
            sub_parser.setInput(lexer.getTokens());
            sub_parser.buildTree();

            // Finally merge the result with this tree
            merge(sub_parser.getTree()->root);
        }
        catch (Exception ex)
        {
            error("failed to load file: " + filename + " with include");
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

    std::shared_ptr<Branch> func_arguments = std::shared_ptr<Branch>(new Branch("FUNC_ARGUMENTS", ""));
    // Process all the function parameters
    while (true)
    {
        /* If the next token is a keyword then process a variable declaration*/
        peak();
        if (is_peak_type("keyword"))
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

    std::shared_ptr<FuncBranch> func_branch = std::shared_ptr<FuncBranch>(new FuncBranch(this->getCompiler()));
    std::shared_ptr<BODYBranch> body_root = std::shared_ptr<BODYBranch>(new BODYBranch(compiler));
    this->root_scope = body_root;
    // Process the function body
    process_body(body_root);

    // Pop off the body
    pop_branch();

    std::shared_ptr<Branch> body = this->branch;

    // Finally create the function branch and merge it all together
    func_branch->setReturnTypeBranch(func_return_type);
    func_branch->setNameBranch(func_name);
    func_branch->setArgumentsBranch(func_arguments);
    func_branch->setBodyBranch(body);

    // Now push it back to the stack
    push_branch(func_branch);
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

    if (new_scope)
    {
        // Start the local scope.
        start_local_scope(body_root);
    }
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


    if (new_scope)
    {
        // Finish the local scope
        finish_local_scope();
    }
    // Push the body root onto the branch stack
    push_branch(body_root);
}

// All possible body statements

void Parser::process_stmt()
{
    peak();
    if (is_peak_type("keyword"))
    {
        // Check to see if this is an "if" statement
        if (is_peak_value("if"))
        {
            // Process the "if" statement
            process_if_stmt();
        }
        else if (is_peak_value("__asm"))
        {
            process_inline_asm();
            process_semicolon();
        }
        else if (is_peak_value("struct"))
        {
            // Its a structure variable declaration
            process_structure_declaration();
            process_semicolon();
        }
        else if (is_peak_value("while"))
        {
            // This is a "while" statement so process it
            process_while_stmt();
        }
        else if (is_peak_value("for"))
        {
            // This is a "for" statement so process it
            process_for_stmt();
        }
        else if (is_peak_value("return"))
        {
            // This is a "return" statement so process it
            process_return_stmt();
            process_semicolon();
        }
        else
        {
            // This is a variable declaration so process it
            process_variable_declaration();
            process_semicolon();
        }
    }
    else if (is_peak_type("identifier"))
    {
        peak(1);
        if (is_peak_symbol("("))
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
    else if (is_peak_operator("*"))
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
    peak();
    if (is_peak_operator("*"))
    {
        // Shift and pop the pointer operator
        shift_pop();
        // Lets set this variable definition branch as a pointer branch
        var_root->setPointer(true);
    }

    // Process the variable access
    process_variable_access();
    pop_branch();

    identifier_branch = this->branch;

    peak();
    // Check if this is just a variable declaration or that we are also setting it to a value    
    if (is_peak_operator("="))
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

    std::shared_ptr<PTRBranch> ptr_branch = std::shared_ptr<PTRBranch>(new PTRBranch(compiler));

    peak();
    if (is_peak_symbol("("))
    {
        // Process the expression
        process_expression(true);
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
    if (op->getValue() != "=" &&
            op->getValue() != "+=" &&
            op->getValue() != "-=" &&
            op->getValue() != "*=" &&
            op->getValue() != "/=")
    {
        error("expecting one of the following operators for assignments: =,+=,-=,*=,/= but " + this->branch_value + " was provided.");
    }


    std::shared_ptr<AssignBranch> assign_branch = std::shared_ptr<AssignBranch>(new AssignBranch(this->getCompiler()));
    assign_branch->setVariableToAssignBranch(left);
    assign_branch->setValueBranch(right);

    // Now finally push the assign branch to the stack
    push_branch(assign_branch);
}

void Parser::process_variable_access(std::shared_ptr<STRUCTDEFBranch> last_struct_def)
{
    std::shared_ptr<VarIdentifierBranch> var_identifier_branch = std::shared_ptr<VarIdentifierBranch>(new VarIdentifierBranch(compiler));
    peak();
    if (is_peak_type("identifier"))
    {
        shift_pop();
        var_identifier_branch->setVariableNameBranch(this->branch);
    }
    else
    {
        error_expecting("identifier", this->token_type);
    }

    // Peak ahead further to check if their is array access
    peak();
    if (is_peak_symbol("["))
    {
        // We have array access process it
        process_array_indexes();
        pop_branch();
        var_identifier_branch->setRootArrayIndexBranch(this->branch);
    }

    // Peak further to see if there is any structure access
    peak();
    if (is_peak_symbol("."))
    {
        process_structure_access();
        pop_branch();
        var_identifier_branch->setStructureAccessBranch(this->branch);
    }

    push_branch(var_identifier_branch);
}

void Parser::process_structure_access()
{
    std::shared_ptr<STRUCTAccessBranch> struct_access_root = NULL;

    // Check to see if this structure access is valid
    peak();
    if (!is_peak_symbol("."))
    {
        error_expecting(".", this->peak_token_value);
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

void Parser::process_expression(bool strict_mode)
{
    process_expression_part(strict_mode);
    peak();
    // Do we have a logical operator if so then we have more to do
    if (compiler->isLogicalOperator(this->peak_token_value))
    {
        // Ok we do shift and pop it off
        shift_pop();
        std::shared_ptr<Branch> op = this->branch;

        // Pop off the previous expression from the stack
        pop_branch();
        std::shared_ptr<Branch> left = this->branch;

        // Now process the new expression
        process_expression(strict_mode);

        // Pop it off
        pop_branch();
        std::shared_ptr<Branch> right = this->branch;

        // Put it all together
        std::shared_ptr<Branch> exp = std::shared_ptr<Branch>(new Branch("E", op->getValue()));
        exp->addChild(left);
        exp->addChild(right);
        push_branch(exp);
    }

}

void Parser::process_expression_part(bool strict_mode)
{
    std::shared_ptr<Branch> exp_root = NULL;
    std::shared_ptr<Branch> left = NULL;
    std::shared_ptr<Branch> op = NULL;
    std::shared_ptr<Branch> right = NULL;

    while (true)
    {
        if (left != NULL && op != NULL && op->getValue() == "=")
        {
            // Ok this is an assignment expression
            // It is important to process the expression for the right branch otherwise we will only end up with one attribute on the stack
            process_expression();
            pop_branch();
            right = this->branch;
            process_assignment(left, right, op);
            pop_branch();
            left = this->branch;
            op = NULL;
            right = NULL;

        }
        else if (left != NULL && op != NULL && right != NULL)
        {
            if (op->getValue() != "*" && op->getValue() != "/")
            {
                // Check to see if BODMAS applies
                peak();
                if (is_peak_operator("*")
                        || is_peak_operator("/"))
                {
                    std::shared_ptr<Branch> l = right;

                    // Shift and pop the operator
                    shift_pop();
                    std::shared_ptr<Branch> o = this->branch;
                    // Shift and pop the right
                    shift_pop();
                    std::shared_ptr<Branch> r = this->branch;

                    exp_root = std::shared_ptr<Branch>(new Branch("E", o->getValue()));
                    exp_root->addChild(l);
                    exp_root->addChild(r);
                    right = exp_root;
                }
            }

            exp_root = std::shared_ptr<Branch>(new Branch("E", op->getValue()));
            exp_root->addChild(left);
            exp_root->addChild(right);

            // Set the left to exp_root, and the op and right to NULL ready for future expressions
            left = exp_root;
            op = NULL;
            right = NULL;
        }
        else
        {
            peak();
            // If the next token is one of the following then set either the left or right branches, which ever one of them is free
            if (is_peak_type("number") ||
                    is_peak_type("identifier") ||
                    is_peak_type("string")
                    )
            {
                handle_left_or_right(&left, &right);
            }
            else if (is_peak_type("operator"))
            {
                if (is_peak_operator("=") && strict_mode)
                {
                    // Strict mode is active assignments are not allowed while strict mode is active and we must break!
                    break;
                }

                if (left == NULL || op != NULL)
                {
                    handle_left_or_right(&left, &right);
                    continue;
                }

                // Logical operators should cause us to break out of this loop so it can be handled in the process_expression method.
                if (compiler->isLogicalOperator(this->peak_token_value))
                {
                    break;
                }

                shift_pop();
                op = this->branch;

                // Check to see if compare expressions order of operations applies.
                if (compiler->isCompareOperator(op->getValue()))
                {
                    // Process the further expression
                    process_expression_part(strict_mode);
                    // Pop off the result
                    pop_branch();
                    // Put it on the right branch
                    right = this->branch;
                }
            }
            else if (is_peak_symbol("("))
            {
                // Pop the left bracket we don't need it anymore
                shift_pop();

                // Recall this method to handle the expression
                process_expression(strict_mode);

                // Pop off the result
                pop_branch();

                // Set either the left or right branch to the result
                if (left == NULL)
                {
                    left = this->branch;
                }
                else if (right == NULL)
                {
                    right = this->branch;
                }

                shift_pop();
                // Is the next symbol a right bracket?
                if (!is_peak_symbol(")"))
                {
                    error("expecting right bracket to end expression");
                }
            }
            else
            {
                break;
            }
        }
    }

    // The expression was never complete so it must be just a number
    if (exp_root == NULL)
    {
        // Check for an error with the expression
        if (left == NULL)
        {

            error("invalid expression");
        }
        exp_root = left;
    }

    // Ok finally lets push the expression root to the stack
    push_branch(exp_root);
}

std::shared_ptr<Branch> Parser::process_expression_operand()
{
    peak();
    std::shared_ptr<Branch> b = NULL;
    if (is_peak_type("number"))
    {
        // Shift and pop the number
        shift_pop();
        b = this->branch;
    }
    else if (is_peak_type("identifier"))
    {
        peak(1);
        // Their is a left bracket so this must be a function call
        if (is_peak_symbol("("))
        {
            process_function_call();
            // Pop the result from the stack
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
    else if (is_peak_operator("&"))
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
    else if (is_peak_operator("*"))
    {
        process_ptr();
        pop_branch();
        b = this->branch;
    }
    else if (is_peak_type("string"))
    {
        // We have a string shift and pop the string 

        shift_pop();
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

void Parser::process_function_call()
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
        peak();
        if (is_peak_symbol(")"))
        {
            // Right curly was found so we are done
            // Pop it off then break
            shift_pop();
            break;
        }
        else if (is_peak_symbol(","))
        {
            // Their is a comma so just pop it off
            shift_pop();
        }
        else
        {
            // Ok lets process the expression

            process_expression();
            // Pop the resulting expression
            pop_branch();
            // Add it to the params
            params->addChild(this->branch);
        }
    }


    // We have everything we need now build the function call
    std::shared_ptr<FuncCallBranch> func_call_root = std::shared_ptr<FuncCallBranch>(new FuncCallBranch(this->getCompiler()));
    func_call_root->setFuncNameBranch(func_name);
    func_call_root->setFuncParamsBranch(params);
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
    std::shared_ptr<Branch> if_body = this->branch;

    std::shared_ptr<IFBranch> if_stmt = std::shared_ptr<IFBranch>(new IFBranch(this->getCompiler()));
    if_stmt->setExpressionBranch(if_exp);
    if_stmt->setBodyBranch(if_body);

    // Check for an else statement
    peak();
    if (is_peak_keyword("else"))
    {
        // Shift and pop the else keyword we do not need it anymore
        shift_pop();

        // Peak to see if its an else if statement
        peak();
        if (is_peak_keyword("if"))
        {
            // No need to shift and pop the "if" keyword as the "process_if_stmt()" method requires it present
            process_if_stmt();
            // Pop off the if statement
            pop_branch();

            // Add the else if branch to this if statement branch.
            if_stmt->setElseIfBranch(this->branch);
        }
        else
        {

            std::shared_ptr<ELSEBranch> else_stmt = std::shared_ptr<ELSEBranch>(new ELSEBranch(this->getCompiler()));
            // Process the body of the else statement
            process_body();
            // Pop the result off the stack
            pop_branch();
            // Add the body to the else statement
            else_stmt->setBodyBranch(this->branch);
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
    // Peak ahead do we have a semicolon? if so we are done otherwise their is an expression that is being returned
    peak();
    if (!is_peak_symbol(";"))
    {
        // We do not have a semicolon so their is an expression to parse
        process_expression();
        // Pop off the result
        pop_branch();
        exp = this->branch;
    }

    // Create the return branch
    std::shared_ptr<Branch> return_branch = std::shared_ptr<Branch>(new Branch("RETURN", ""));
    // If their was an expression then we need to add it to the return branch
    if (exp != NULL)
    {

        return_branch->addChild(exp);
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
    peak();
    if (is_peak_operator("*"))
    {
        // Shift and pop the operator from the stack as we do not need it anymore
        shift_pop();
        struct_declaration->setPointer(true);
    }

    // process the variable access
    process_variable_access();
    pop_branch();

    std::shared_ptr<Branch> identifier_branch = this->branch;

    std::shared_ptr<Branch> var_value_branch = NULL;
    // Peak ahead to see if their is an = sign it may be a structure declaration
    peak();
    if (is_peak_operator("="))
    {
        // Ok their is an equal sign so we must be assigning this declaration
        // Shift and pop the equal sign we no longer need it
        shift_pop();

        // Peak further to check if we have an identifier
        peak();
        if (is_peak_type("identifier"))
        {
            // Shift and pop the identifier from the stack
            shift_pop();
            var_value_branch = this->branch;
        }
        else
        {

            error_expecting("identifier", this->peak_token_value);
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
    std::shared_ptr<Branch> body = this->branch;

    // Time to put it all together
    std::shared_ptr<Branch> while_stmt = std::shared_ptr<Branch>(new Branch("WHILE", ""));
    while_stmt->addChild(exp);
    while_stmt->addChild(body);

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
    peak();

    // Start the "for_stmt" local scope.
    start_local_scope(for_stmt);
    if (is_peak_type("keyword") || is_peak_type("identifier"))
    {
        if (is_peak_type("keyword"))
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
    peak();
    if (!is_peak_symbol(";"))
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
    peak();
    if (!is_peak_symbol(")"))
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
    process_body(NULL, false);
    // Pop the resulting body from the stack
    pop_branch();

    body = std::dynamic_pointer_cast<BODYBranch>(this->branch);

    // Put it all together
    for_stmt->setInitBranch(init_var);
    for_stmt->setCondBranch(cond_exp);
    for_stmt->setLoopBranch(loop_stmt);
    for_stmt->setBodyBranch(body);

    // Finish the scope for the "for_stmt"
    finish_local_scope();

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
    peak();
    if (is_peak_symbol("["))
    {
        // Yes there are more indexes to go so recall ourself

        process_array_indexes();
        pop_branch();
        // Pop the result and attach it as a child to our array index branch
        array_index_branch->setNextArrayIndexBranch(this->branch);
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
    shift_pop();
    if (!is_branch_type("identifier"))
    {

        error("expecting an identifier, however token: \"" + this->token_value + "\" was provided");
    }
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
    error("peak failed, no more input with unfinished parse, check your source file.", false);
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

void Parser::push_branch(std::shared_ptr<Branch> branch)
{
    // Before pushing we must assign the root branch and the scopes to the branch
    setRootAndScopes(branch);
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

bool Parser::is_peak_symbol(std::string symbol)
{

    return is_peak_type("symbol") && is_peak_value(symbol);
}

bool Parser::is_peak_symbol(std::string symbol, int peak)
{
    if (peak < this->input.size())
    {
        std::shared_ptr<Token> peak_token = this->input.at(peak);
        if (peak_token->getType() == "symbol" && peak_token->getValue() == symbol)
        {
            return true;
        }
    }
    else
    {

        error("bool Parser::is_peak_type(std::string type, int peak): peak offset is breaching bounds.");
    }

    return false;
}

bool Parser::is_peak_type(std::string type)
{

    return this->peak_token_type == type;
}

bool Parser::is_peak_type(std::string type, int peak)
{
    if (peak < this->input.size())
    {
        std::shared_ptr<Token> peak_token = this->input.at(peak);
        if (peak_token->getType() == type)
        {
            return true;
        }
    }
    else
    {

        error("bool Parser::is_peak_type(std::string type, int peak): peak offset is breaching bounds.");
    }

    return false;
}

bool Parser::is_peak_value(std::string value)
{

    return this->peak_token_value == value;
}

bool Parser::is_peak_keyword(std::string keyword)
{

    return is_peak_type("keyword") && is_peak_value(keyword);
}

bool Parser::is_peak_operator(std::string op)
{

    return is_peak_type("operator") && is_peak_value(op);
}

bool Parser::is_peak_operator(std::string op, int peak)
{
    if (peak < this->input.size())
    {
        std::shared_ptr<Token> peak_token = this->input.at(peak);
        if (peak_token->getType() == "operator" &&
                peak_token->getValue() == op)
        {
            return true;
        }
    }
    else
    {

        error("bool Parser::is_peak_operator(std::string op, int peak): peak offset is breaching bounds.");
    }

    return false;
}

bool Parser::is_peak_identifier(std::string identifier)
{

    return is_peak_type("identifier") && is_peak_value(identifier);
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