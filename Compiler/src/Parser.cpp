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
        else
        {
            peak(1);
            if (is_peak_type("identifier"))
            {
                // Check to see if this is a function or a variable declaration
                peak(2);
                if (is_peak_operator("=") || is_peak_symbol(";"))
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


    // Process the function body
    process_body();

    // Pop off the body
    pop_branch();

    std::shared_ptr<Branch> body = this->branch;

    // Finally create the function branch and merge it all together
    std::shared_ptr<FuncBranch> func_branch = std::shared_ptr<FuncBranch>(new FuncBranch(this->getCompiler()));
    func_branch->setReturnTypeBranch(func_return_type);
    func_branch->setNameBranch(func_name);
    func_branch->setArgumentsBranch(func_arguments);
    func_branch->setBodyBranch(body);

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
    if (is_peak_type("keyword"))
    {
        // Check to see if this is an "if" statement
        if (is_peak_value("if"))
        {
            // Process the "if" statement
            process_if_stmt();
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
    else if(is_peak_type("operator"))
    {
        peak(1);
        if (is_peak_type("identifier"))
        {
            peak(2);
            if (is_peak_type("operator"))
            {
                // This should be a pointer value assignment
                process_assignment();
                process_semicolon();
            }
        }
    }
    else if (is_peak_type("identifier"))
    {
        peak(1);
        if (is_peak_type("operator"))
        {
            // This is an assignment
            process_assignment();
            process_semicolon();
        }
        else if (is_peak_symbol("("))
        {
            // A left bracket was found so it must be a function call
            process_function_call();
            process_semicolon();
        }
        else
        {
            shift_pop();
            error("unexpected token: " + this->branch_value + ", body's allow statements and variable declarations only.");
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
    std::shared_ptr<Branch> var_name = NULL;
    std::shared_ptr<Branch> var_keyword = NULL;
    std::shared_ptr<Branch> var_value = NULL;
    bool is_pointer = false;

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

    var_keyword = this->branch;

    // Shift the identifier or "*" symbol of the variable onto the stack
    shift_pop();
    if (is_branch_operator("*"))
    {
        // Its actually a pointer so just set a boolean so we know
        is_pointer = true;

        // Shift and pop the next token as it will be the variable name, handled below
        shift_pop();
    }

    if (!is_branch_type("identifier"))
    {
        error_expecting("identifier", this->branch_value);
    }
    var_name = this->branch;

    // Check if this is just a variable declaration or that we are also setting it to a value
    peak();
    if (is_peak_operator("="))
    {
        // Their is an equal sign so we are setting it to something
        // Shift and pop that "=" sign we do not need it anymore
        shift_pop();

        // Process the expression we are setting this variable too
        process_expression();
        // Pop the expression from the stack
        pop_branch();
        var_value = this->branch;
    }

    /* 
     * Now that we have popped to variable name and keyword of the variable
     * we need to create a branch for, lets create a branch for them  */

    std::shared_ptr<VDEFBranch> var_root;
    if (is_pointer)
    {
        var_root = std::shared_ptr<VDEFPTRBranch>(new VDEFPTRBranch(this->getCompiler()));
    }
    else
    {
        var_root = std::shared_ptr<VDEFBranch>(new VDEFBranch(this->getCompiler()));
    }

    var_root->setKeywordBranch(var_keyword);
    var_root->setNameBranch(var_name);
    var_root->setValueExpBranch(var_value);


    // Push that root back to the branches
    push_branch(var_root);

}

void Parser::process_assignment()
{
    // Process the variable access
    process_variable_access();
    // Pop the result from the stack
    pop_branch();

    std::shared_ptr<Branch> dst_branch = this->branch;
    shift_pop();
    // Check for a valid assignment, e.g =, +=, -=
    if (!is_branch_operator("=") &&
            !is_branch_operator("+=") &&
            !is_branch_operator("-=") &&
            !is_branch_operator("*=") &&
            !is_branch_operator("/="))
    {
        error("expecting one of the following operators for assignments: =,+=,-=,*=,/=");
    }

    std::shared_ptr<Branch> op = this->branch;

    process_expression();
    // Pop the expression from the stack
    pop_branch();
    std::shared_ptr<Branch> expression = this->branch;

    std::shared_ptr<AssignBranch> assign_branch = std::shared_ptr<AssignBranch>(new AssignBranch(this->getCompiler()));
    assign_branch->setVariableToAssignBranch(dst_branch);
    assign_branch->setValueBranch(expression);

    // Now finally push the assign branch to the stack
    push_branch(assign_branch);
}

void Parser::process_variable_access()
{
    std::shared_ptr<Branch> root = NULL;
    shift_pop();
     // Check for pointer access

    if (is_branch_operator("@"))
    {
        root = std::shared_ptr<Branch>(new Branch("PTR", ""));
        // Shift and pop the next identifier which is the variable to access
        shift_pop();
    }

    // Make sure we have an identifier
    if (!is_branch_type("identifier"))
    {
        error_expecting("identifier", this->branch_type);
    }

    std::shared_ptr<Branch> identifier = this->branch;
    // Do we have a root?
    if (root != NULL)
    {
        // Well add this identifier to it.
        root->addChild(identifier);
    }
    else
    {
        // No root? Ok set the root to the identifier
        root = identifier;
    }

    // Push the root branch to the stack
    push_branch(root);
}


void Parser::process_structure_access()
{
    std::shared_ptr<Branch> struct_access_root = NULL;
    bool dot_present = false;
    std::shared_ptr<Branch> left = NULL;
    std::shared_ptr<Branch> right = NULL;
    
    while(true)
    {
        shift_pop();
        if (is_branch_type("identifier"))
        {
            if (left == NULL)
            {
                left = this->branch;
            }
            else if(right == NULL)
            {
                right = this->branch;
            }
        }
        else if(is_branch_symbol("."))
        {
            dot_present = true;
        }
        else
        {
            break;
        }
        
        
        if (left != NULL && dot_present && right != NULL)
        {
            struct_access_root = std::shared_ptr<Branch>(new Branch("STRUCT_ACCESS", ""));
            struct_access_root->addChild(left);
            struct_access_root->addChild(right);
            
            left = struct_access_root;
            dot_present = false;
            right = NULL;
        }
    }
    
    // Push the branch to the tree
    push_branch(struct_access_root);
   
}

void Parser::process_expression()
{
    std::shared_ptr<Branch> exp_root = NULL;
    std::shared_ptr<Branch> left = NULL;
    std::shared_ptr<Branch> op = NULL;
    std::shared_ptr<Branch> right = NULL;

    while (true)
    {
        peak();
        // If the next token is one of the following then set either the left or right branches, which ever one of them is free
        if (is_peak_type("number") ||
                is_peak_type("identifier") ||
                is_peak_type("string") ||
                // This is used for addresses, e.g ?test get the address of variable test
                is_peak_operator("?") ||
                // This is used for pointer access, e.g @a
                is_peak_operator("@")
                )
        {
            if (left == NULL)
            {
                left = this->process_expression_operand();
            }
            else if (right == NULL)
            {
                right = this->process_expression_operand();
            }
        }
        else if (is_peak_type("operator"))
        {
            shift_pop();
            op = this->branch;
            
            // Check to see if compare expressions order of operations applies.
            if (compiler->isCompareOperator(this->peak_token_value))
            {
                // Process the further expression
                process_expression();
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
            process_expression();

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

        if (left != NULL && op != NULL && right != NULL)
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
        else if (is_peak_symbol("."))
        {
            // We must be accessing a structure element
            process_structure_access();
            // Pop the result from the stack
            pop_branch();
            b = this->branch;
        }
        else
        {
            // Shift and pop the identifier
            shift_pop();
            b = this->branch;
        }
    }
    else if (is_peak_operator("?"))
    {
        // We are getting the address of a declaration here
        // Shift and pop the "?" symbol we do not need it anymore
        shift_pop();

        // Shift and pop the identifier
        shift_pop();
        std::shared_ptr<AddressOfBranch> address_of_branch = std::shared_ptr<AddressOfBranch>(new AddressOfBranch(this->getCompiler()));
        address_of_branch->setVariableBranch(this->branch);
        b = address_of_branch;
    }
    else if(is_peak_type("operator"))
    {
        // Peak further and see if the next value is an identifier
        peak(1);
        if (is_peak_type("identifier"))
        {
            /* Ok this is accessing a variable this could be a pointer 
             * e.g "*a" get the value from the memory location pointer "a" is pointing to
             */
            process_variable_access();
            // Pop off the result
            pop_branch();
            b = this->branch;
        }
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

    std::shared_ptr<Branch> if_stmt = std::shared_ptr<Branch>(new Branch("IF", ""));
    if_stmt->addChild(if_exp);
    if_stmt->addChild(if_body);

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

            // Add it to this "if" statement
            if_stmt->addChild(this->branch);
        }
        else
        {
            std::shared_ptr<Branch> else_stmt = std::shared_ptr<Branch>(new Branch("ELSE", ""));
            // Process the body of the else statement
            process_body();
            // Pop the result off the stack
            pop_branch();
            // Add the body to the else statement
            else_stmt->addChild(this->branch);
            // Add the else statement to the if statement
            if_stmt->addChild(else_stmt);
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
    std::shared_ptr<Branch> struct_body = this->branch;

    // Create the structure branch
    std::shared_ptr<Branch> struct_root = std::shared_ptr<Branch>(new Branch("STRUCT", ""));
    // Add the structure name to the structure
    struct_root->addChild(struct_name);
    // Add the body to the structure
    struct_root->addChild(struct_body);

    // Finally add the structure root to the main tree
    push_branch(struct_root);
}

void Parser::process_structure_declaration()
{
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

    std::shared_ptr<Branch> struct_name = this->branch;

    // Shift and pop the token and check that it is an "identifier" this is the variable name
    shift_pop();
    if (!is_branch_type("identifier"))
    {
        error_expecting("identifier", this->token_value);
    }

    std::shared_ptr<Branch> var_name = this->branch;

    std::shared_ptr<Branch> var_value = NULL;
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
            var_value = this->branch;
        }
        else
        {
            error_expecting("identifier", this->peak_token_value);
        }
    }

    // Create the structure variable declaration
    std::shared_ptr<Branch> struct_declaration = std::shared_ptr<Branch>(new Branch("STRUCT_DEF", ""));
    struct_declaration->addChild(struct_name);
    struct_declaration->addChild(var_name);
    // Does this structure variable declaration also have an assignment?
    if (var_value != NULL)
    {
        // Add the value to the structure declaration branch
        struct_declaration->addChild(var_value);
    }
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
    std::shared_ptr<Branch> init_var = NULL;
    std::shared_ptr<Branch> cond_exp = NULL;
    std::shared_ptr<Branch> loop_stmt = NULL;
    std::shared_ptr<Branch> body = NULL;

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
            // Process the variable assignment
            process_assignment();
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
        // Process the assignment
        process_assignment();
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
    // Pop the resulting body from the stack
    pop_branch();
    body = this->branch;

    // Put it all together
    std::shared_ptr<Branch> for_stmt = std::shared_ptr<Branch>(new Branch("FOR", ""));
    std::shared_ptr<Branch> init = std::shared_ptr<Branch>(new Branch("INIT", ""));
    std::shared_ptr<Branch> cond = std::shared_ptr<Branch>(new Branch("COND", ""));
    std::shared_ptr<Branch> loop = std::shared_ptr<Branch>(new Branch("LOOP", ""));

    if (init_var != NULL)
    {
        init->addChild(init_var);
    }
    if (cond_exp != NULL)
    {
        cond->addChild(cond_exp);
    }
    if (loop_stmt != NULL)
    {
        loop->addChild(loop_stmt);
    }

    for_stmt->addChild(init);
    for_stmt->addChild(cond);
    for_stmt->addChild(loop);
    for_stmt->addChild(body);

    // Push the for statement to the stack
    push_branch(for_stmt);
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

bool Parser::is_peak_type(std::string type)
{
    return this->peak_token_type == type;
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

bool Parser::is_peak_identifier(std::string identifier)
{
    return is_peak_type("identifier") && is_peak_value(identifier);
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