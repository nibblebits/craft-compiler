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
 * Description: The parser and assembler for the 8086 code generator,
 * note that no checking of input is done here as the assembly is generated from the code generator
 * so it is expected to be valid.
 */

#include "Assembler8086.h"
#include "common.h"
#include "def.h"
#include "LabelBranch.h"
#include "InstructionBranch.h"
#include "SegmentBranch.h"

Assembler8086::Assembler8086(Compiler* compiler, std::shared_ptr<VirtualObjectFormat> object_format) : Assembler(compiler, object_format)
{
    Assembler::addKeyword("segment");
    Assembler::addKeyword("extern");
    Assembler::addKeyword("global");

    Assembler::addRegister("ax");
    Assembler::addRegister("ah");
    Assembler::addRegister("al");
    Assembler::addRegister("cx");
    Assembler::addRegister("ch");
    Assembler::addRegister("cl");
    Assembler::addRegister("dx");
    Assembler::addRegister("dh");
    Assembler::addRegister("dl");
    Assembler::addRegister("bx");
    Assembler::addRegister("bh");
    Assembler::addRegister("bl");

    Assembler::addRegister("di");
    Assembler::addRegister("si");
    Assembler::addRegister("bp");
    Assembler::addRegister("sp");

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
    Assembler::addInstruction("lea");
    Assembler::addInstruction("call");
    Assembler::addInstruction("ret");
}

Assembler8086::~Assembler8086()
{
}

std::shared_ptr<Branch> Assembler8086::parse()
{
#ifdef DEBUG_MODE
    debug_output_tokens(Assembler::getTokens());
#endif

    root = std::shared_ptr<Branch>(new Branch("root", ""));
    while (hasTokens())
    {
        parse_part();
    }

    // Lets get all the branches
    while (hasBranches())
    {
        pop_front_branch();
        root->addChild(getPoppedBranch());
    }

#ifdef DEBUG_MODE
    debug_output_branch(root);
#endif

    return root;

}

void Assembler8086::exp_handler()
{
    shift();
}

void Assembler8086::left_exp_handler()
{
    exp_handler();
}

void Assembler8086::right_exp_handler()
{
    exp_handler();
}

std::shared_ptr<InstructionBranch> Assembler8086::new_ins_branch()
{
    return std::shared_ptr<InstructionBranch>(new InstructionBranch(getCompiler()));
}

void Assembler8086::parse_part()
{
    if (is_next_segment())
    {
        parse_segment();
    }
    else if (is_next_label())
    {
        parse_label();
    }
    else if (is_next_instruction())
    {
        parse_ins();
    }
    else
    {
        peek();
        throw AssemblerException("void Assembler8086::parse_part():  unexpected token \"" + getPeakTokenValue() + "\" this instruction or syntax may not be implemented.");
    }

}

void Assembler8086::parse_segment()
{
    // Shift and pop the segment name we don't need it anymore
    shift_pop();

    // Next up is the segment name, we need this
    shift_pop();
    std::shared_ptr<Branch> segment_name_branch = Assembler::getPoppedBranch();

    std::shared_ptr<SegmentBranch> segment_root = std::shared_ptr<SegmentBranch>(new SegmentBranch(getCompiler()));
    segment_root->setSegmentNameBranch(segment_name_branch);

    // Create the contents branch and add it to the segment branch
    std::shared_ptr<Branch> contents_branch = std::shared_ptr<Branch>(new Branch("CONTENTS", ""));
    segment_root->setContentsBranch(contents_branch);

    while (hasTokens())
    {
        if (is_next_segment())
        {
            /* We should stop now as we are at another segment, segments are independent from other segments*/
            break;
        }

        parse_part();
        pop_branch();
        // Add the branch to the segment contents branch
        contents_branch->addChild(getPoppedBranch());
    }

    push_branch(segment_root);

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
    label_branch->setContentsBranch(label_contents_branch);

    while (hasTokens())
    {
        if (is_next_label() ||
                is_next_segment())
        {
            /* We should stop now as we are at another label or segment
             all labels and segments should be independent from other labels and segments.*/
            break;
        }

        parse_part();
        pop_branch();
        // Add the branch to the label contents branch
        label_contents_branch->addChild(getPoppedBranch());
    }

    push_branch(label_branch);
}

void Assembler8086::parse_ins()
{
    std::shared_ptr<Branch> name_branch = NULL;
    std::shared_ptr<Branch> dest_exp = NULL;
    std::shared_ptr<Branch> source_exp = NULL;

    // Shift and pop the instruction
    shift_pop();
    name_branch = getPoppedBranch();

    // Do we have an expression
    peek();
    if (is_peek_type("identifier")
            || is_peek_type("number")
            || is_peek_type("register"))
    {
        // Next will be the left operand
        parse_expression();
        // Pop it off
        pop_branch();
        dest_exp = getPoppedBranch();

        // Do we have a second operand?
        peek();
        if (is_peek_symbol(","))
        {
            // Now we need to shift and pop off the comma ","
            shift_pop();

            // Finally a final expression which will be the second operand
            parse_expression();

            // Pop it off
            pop_branch();
            source_exp = getPoppedBranch();
        }
    }

    // Put it all together
    std::shared_ptr<InstructionBranch> ins_branch = std::shared_ptr<InstructionBranch>(new InstructionBranch(getCompiler()));
    ins_branch->setInstructionNameBranch(name_branch);
    ins_branch->setLeftBranch(dest_exp);
    ins_branch->setRightBranch(source_exp);

    // Push the finished branch to the stack
    push_branch(ins_branch);
}

bool Assembler8086::is_next_segment()
{
    peek();
    return (is_peek_type("keyword")
            && is_peek_value("segment"));
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

bool Assembler8086::is_next_instruction()
{
    peek();
    return is_peek_type("instruction");
}

void Assembler8086::generate()
{
    for (std::shared_ptr<Branch> branch : root->getChildren())
    {
        if (branch->getType() == "SEGMENT")
        {
            generate_segment(std::dynamic_pointer_cast<SegmentBranch>(branch));
        }
        else
        {
            throw new AssemblerException("void Assembler8086::generate(): branch requires a segment.");
        }
    }
}

void Assembler8086::generate_part(std::shared_ptr<Branch> branch, std::shared_ptr<VirtualSegment> segment)
{
    Stream* stream = segment->getStream();
    if (branch->getType() == "INSTRUCTION")
    {
        generate_instruction(std::dynamic_pointer_cast<InstructionBranch>(branch), segment);
    }

}

void Assembler8086::generate_instruction(std::shared_ptr<InstructionBranch> instruction_branch, std::shared_ptr<VirtualSegment> segment)
{
    std::shared_ptr<Branch> instruction_name_branch = instruction_branch->getInstructionNameBranch();
    std::string instruction_name = instruction_name_branch->getValue();
    if (instruction_name == "mov")
    {
        // This is a move instruction
    }
}

void Assembler8086::generate_segment(std::shared_ptr<SegmentBranch> branch)
{
    std::shared_ptr<VirtualObjectFormat> obj_format = Assembler::getObjectFormat();
    std::shared_ptr<VirtualSegment> segment = obj_format->createSegment(branch->getSegmentNameBranch()->getValue());
    for (std::shared_ptr<Branch> child : branch->getContentsBranch()->getChildren())
    {
        generate_part(child, segment);
    }

}