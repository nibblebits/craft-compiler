#include "GoblinByteCodeGenerator.h"
#include "GoblinByteCodeLinker.h"
GoblinByteCodeGenerator::GoblinByteCodeGenerator(Compiler* compiler) : CodeGenerator(compiler, "goblin_bytecode")
{
    this->linker = std::shared_ptr<Linker>(new GoblinByteCodeLinker(compiler));
}
GoblinByteCodeGenerator::~GoblinByteCodeGenerator()
{
    
}

void GoblinByteCodeGenerator::scope_start(std::shared_ptr<Branch> branch)
{
    
}
void GoblinByteCodeGenerator::scope_assignment(std::shared_ptr<struct scope_variable> var, std::shared_ptr<Branch> assign_root, std::shared_ptr<Branch> assign_to)
{

}
void GoblinByteCodeGenerator::scope_func_call(std::shared_ptr<Branch> branch, std::string func_name, std::vector<std::shared_ptr < Branch>> func_arguments)
{
    
}
void GoblinByteCodeGenerator::scope_end(std::shared_ptr<Branch> branch)
{
    
}
void GoblinByteCodeGenerator::scope_exp_start()
{
    
}
void GoblinByteCodeGenerator::scope_handle_exp(std::shared_ptr<Branch> branch)
{
    
}
void GoblinByteCodeGenerator::scope_exp_end()
{
    
}
void GoblinByteCodeGenerator::scope_handle_number(std::shared_ptr<Branch> branch)
{
    
}
void GoblinByteCodeGenerator::scope_handle_identifier(std::shared_ptr<Branch> branch)
{
    
}
void GoblinByteCodeGenerator::scope_handle_inline_asm(std::shared_ptr<Branch> branch)
{
    
}
void GoblinByteCodeGenerator::scope_struct_assign_start(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> struct_ins_var, std::shared_ptr<struct scope_variable> struct_attr_var)
{
    
}
void GoblinByteCodeGenerator::scope_struct_assign_end(std::shared_ptr<Branch> branch, std::shared_ptr<struct scope_variable> struct_ins_var, std::shared_ptr<struct scope_variable> struct_attr_var)
{
    
}

std::shared_ptr<Linker> GoblinByteCodeGenerator::getLinker()
{
    return this->linker;
}