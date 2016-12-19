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
 * File:   Assembler8086.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 December 2016, 16:44
 * 
 * Description: 
 */

#include "Assembler8086.h"
#include "common.h"
#include "def.h"
#include "LabelBranch.h"

Assembler8086::Assembler8086(Compiler* compiler) : Assembler(compiler)
{
    Assembler::addKeyword("extern");
    Assembler::addKeyword("global");

    // Not all the instructions that are implemented, but enough for now
    Assembler::addInstruction("mov");
    Assembler::addInstruction("push");
    Assembler::addInstruction("pop");
    Assembler::addInstruction("add");
    Assembler::addInstruction("sub");
    Assembler::addInstruction("mul");
    Assembler::addInstruction("div");
    Assembler::addInstruction("xor");
    Assembler::addInstruction("and");
    Assembler::addInstruction("or");
    Assembler::addInstruction("int");
    Assembler::addInstruction("call");
}

Assembler8086::~Assembler8086()
{
}

std::shared_ptr<Branch> Assembler8086::parse()
{
#ifdef DEBUG_MODE
    debug_output_tokens(Assembler::getTokens());
#endif

    std::shared_ptr<Branch> root = std::shared_ptr<Branch>(new Branch("root", ""));
    while (hasTokens())
    {
        parse_part();
    }

    // Lets get all the branches
    while (hasBranches())
    {
        pop_branch();
        root->addChild(getPoppedBranch());
    }

#ifdef DEBUG_MODE
    debug_output_branch(root);
#endif

    return root;

}

void Assembler8086::parse_part()
{
    if (is_next_label())
    {
        parse_label();
    }
    else if (is_next_mov_ins())
    {
        parse_mov_ins();
    }

}

void Assembler8086::parse_label()
{
    // Shift and pop the label name
    shift_pop();
    std::shared_ptr<Branch> label_name_branch = Assembler::getPoppedBranch();
    // Shift and pop the symbol ":" we do not need it anymore
    shift_pop();

    // Create the label branch
    std::shared_ptr<LabelBranch> label_branch = std::shared_ptr<LabelBranch>(new LabelBranch(getCompiler()));
    label_branch->setLabelNameBranch(label_name_branch);

    // Create the label contents branch and add it to the label branch
    std::shared_ptr<Branch> label_contents_branch = std::shared_ptr<Branch>(new Branch("CONTENTS", ""));
    label_branch->addChild(label_contents_branch);

    while (hasTokens())
    {
        if (is_next_label())
        {
            /* We should stop now as we are at another label 
             all labels should be independent from other labels.*/
            break;
        }

        parse_part();
        pop_branch();
        // Add the branch to the label contents branch
        label_contents_branch->addChild(getPoppedBranch());
    }

    push_branch(label_branch);
}

void Assembler8086::parse_mov_ins()
{
    // Shift and pop the "mov" instruction we don't need it anymore
    shift_pop();
    
}

bool Assembler8086::is_next_label()
{
    peek();
    if (is_peek_type("identifier"))
    {
        peek(1);
        if (is_peek_symbol(":"))
        {
            // This is a label
            return true;
        }
    }

    return false;
}

bool Assembler8086::is_next_mov_ins()
{
    peek();
    if (is_peek_type("instruction")
            && is_peek_value("mov"))
    {
        return true;
    }

    return false;
}

void Assembler8086::generate()
{

}

