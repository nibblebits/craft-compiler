#include "GoblinByteCodeGenerator.h"
#include "GoblinByteCodeLinker.h"
#include "branches.h"

GoblinByteCodeGenerator::GoblinByteCodeGenerator(Compiler* compiler) : CodeGenerator(compiler, "goblin_bytecode")
{
    this->linker = std::shared_ptr<Linker>(new GoblinByteCodeLinker(compiler));
}

GoblinByteCodeGenerator::~GoblinByteCodeGenerator()
{

}

void GoblinByteCodeGenerator::scope_start(std::shared_ptr<Branch> branch)
{
    // We must make a placeholder so that the stream size offset is correct throughout compiling.
    this->stream->write8(ADDDP);
    this->stream->write32(0);
}

void GoblinByteCodeGenerator::scope_end(std::shared_ptr<Branch> branch)
{
   
    
}

void GoblinByteCodeGenerator::scope_assignment(std::shared_ptr<struct variable> var, std::shared_ptr<Branch> assign_root, std::shared_ptr<Branch> assign_to)
{
    
}

void GoblinByteCodeGenerator::scope_func_call(std::shared_ptr<Branch> branch, std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments)
{

}

void GoblinByteCodeGenerator::scope_handle_exp(std::shared_ptr<Branch> branch)
{

}

void GoblinByteCodeGenerator::scope_handle_inline_asm(std::shared_ptr<Branch> branch)
{

}

std::shared_ptr<Linker> GoblinByteCodeGenerator::getLinker()
{
    return this->linker;
}