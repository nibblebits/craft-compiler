#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=Cygwin-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/ASMArgBranch.o \
	${OBJECTDIR}/src/ASMArgsBranch.o \
	${OBJECTDIR}/src/ASMBranch.o \
	${OBJECTDIR}/src/ASTAssistant.o \
	${OBJECTDIR}/src/AddressOfBranch.o \
	${OBJECTDIR}/src/ArrayIndexBranch.o \
	${OBJECTDIR}/src/Assembler.o \
	${OBJECTDIR}/src/AssignBranch.o \
	${OBJECTDIR}/src/BODYBranch.o \
	${OBJECTDIR}/src/Branch.o \
	${OBJECTDIR}/src/BreakBranch.o \
	${OBJECTDIR}/src/CodeGenerator.o \
	${OBJECTDIR}/src/Compiler.o \
	${OBJECTDIR}/src/CompilerEntity.o \
	${OBJECTDIR}/src/ContinueBranch.o \
	${OBJECTDIR}/src/CustomBranch.o \
	${OBJECTDIR}/src/EBranch.o \
	${OBJECTDIR}/src/ELSEBranch.o \
	${OBJECTDIR}/src/FORBranch.o \
	${OBJECTDIR}/src/FuncArgumentsBranch.o \
	${OBJECTDIR}/src/FuncBranch.o \
	${OBJECTDIR}/src/FuncCallBranch.o \
	${OBJECTDIR}/src/FuncDefBranch.o \
	${OBJECTDIR}/src/Helper.o \
	${OBJECTDIR}/src/IFBranch.o \
	${OBJECTDIR}/src/Lexer.o \
	${OBJECTDIR}/src/Linker.o \
	${OBJECTDIR}/src/Logger.o \
	${OBJECTDIR}/src/LogicalNotBranch.o \
	${OBJECTDIR}/src/MacroDefineBranch.o \
	${OBJECTDIR}/src/MacroDefinitionIdentifierBranch.o \
	${OBJECTDIR}/src/MacroFuncCallBranch.o \
	${OBJECTDIR}/src/MacroIfDefBranch.o \
	${OBJECTDIR}/src/PTRBranch.o \
	${OBJECTDIR}/src/Parser.o \
	${OBJECTDIR}/src/Preprocessor.o \
	${OBJECTDIR}/src/ReturnBranch.o \
	${OBJECTDIR}/src/RootBranch.o \
	${OBJECTDIR}/src/STRUCTAccessBranch.o \
	${OBJECTDIR}/src/STRUCTBranch.o \
	${OBJECTDIR}/src/STRUCTDEFBranch.o \
	${OBJECTDIR}/src/STRUCTDescriptorBranch.o \
	${OBJECTDIR}/src/Scope.o \
	${OBJECTDIR}/src/ScopeBranch.o \
	${OBJECTDIR}/src/SemanticValidator.o \
	${OBJECTDIR}/src/StandardScopeBranch.o \
	${OBJECTDIR}/src/Stream.o \
	${OBJECTDIR}/src/Token.o \
	${OBJECTDIR}/src/Tree.o \
	${OBJECTDIR}/src/TreeImprover.o \
	${OBJECTDIR}/src/VDEFBranch.o \
	${OBJECTDIR}/src/VarIdentifierBranch.o \
	${OBJECTDIR}/src/VirtualObjectFormat.o \
	${OBJECTDIR}/src/VirtualSegment.o \
	${OBJECTDIR}/src/WhileBranch.o \
	${OBJECTDIR}/src/common.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-shared
CXXFLAGS=-shared

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../bin/Compiler.${CND_DLIB_EXT}

../bin/Compiler.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ../bin
	${LINK.cc} -o ../bin/Compiler.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared

${OBJECTDIR}/src/ASMArgBranch.o: src/ASMArgBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ASMArgBranch.o src/ASMArgBranch.cpp

${OBJECTDIR}/src/ASMArgsBranch.o: src/ASMArgsBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ASMArgsBranch.o src/ASMArgsBranch.cpp

${OBJECTDIR}/src/ASMBranch.o: src/ASMBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ASMBranch.o src/ASMBranch.cpp

${OBJECTDIR}/src/ASTAssistant.o: src/ASTAssistant.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ASTAssistant.o src/ASTAssistant.cpp

${OBJECTDIR}/src/AddressOfBranch.o: src/AddressOfBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AddressOfBranch.o src/AddressOfBranch.cpp

${OBJECTDIR}/src/ArrayIndexBranch.o: src/ArrayIndexBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ArrayIndexBranch.o src/ArrayIndexBranch.cpp

${OBJECTDIR}/src/Assembler.o: src/Assembler.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Assembler.o src/Assembler.cpp

${OBJECTDIR}/src/AssignBranch.o: src/AssignBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AssignBranch.o src/AssignBranch.cpp

${OBJECTDIR}/src/BODYBranch.o: src/BODYBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BODYBranch.o src/BODYBranch.cpp

${OBJECTDIR}/src/Branch.o: src/Branch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Branch.o src/Branch.cpp

${OBJECTDIR}/src/BreakBranch.o: src/BreakBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BreakBranch.o src/BreakBranch.cpp

${OBJECTDIR}/src/CodeGenerator.o: src/CodeGenerator.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CodeGenerator.o src/CodeGenerator.cpp

${OBJECTDIR}/src/Compiler.o: src/Compiler.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Compiler.o src/Compiler.cpp

${OBJECTDIR}/src/CompilerEntity.o: src/CompilerEntity.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CompilerEntity.o src/CompilerEntity.cpp

${OBJECTDIR}/src/ContinueBranch.o: src/ContinueBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ContinueBranch.o src/ContinueBranch.cpp

${OBJECTDIR}/src/CustomBranch.o: src/CustomBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CustomBranch.o src/CustomBranch.cpp

${OBJECTDIR}/src/EBranch.o: src/EBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/EBranch.o src/EBranch.cpp

${OBJECTDIR}/src/ELSEBranch.o: src/ELSEBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ELSEBranch.o src/ELSEBranch.cpp

${OBJECTDIR}/src/FORBranch.o: src/FORBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FORBranch.o src/FORBranch.cpp

${OBJECTDIR}/src/FuncArgumentsBranch.o: src/FuncArgumentsBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FuncArgumentsBranch.o src/FuncArgumentsBranch.cpp

${OBJECTDIR}/src/FuncBranch.o: src/FuncBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FuncBranch.o src/FuncBranch.cpp

${OBJECTDIR}/src/FuncCallBranch.o: src/FuncCallBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FuncCallBranch.o src/FuncCallBranch.cpp

${OBJECTDIR}/src/FuncDefBranch.o: src/FuncDefBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FuncDefBranch.o src/FuncDefBranch.cpp

${OBJECTDIR}/src/Helper.o: src/Helper.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Helper.o src/Helper.cpp

${OBJECTDIR}/src/IFBranch.o: src/IFBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/IFBranch.o src/IFBranch.cpp

${OBJECTDIR}/src/Lexer.o: src/Lexer.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Lexer.o src/Lexer.cpp

${OBJECTDIR}/src/Linker.o: src/Linker.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Linker.o src/Linker.cpp

${OBJECTDIR}/src/Logger.o: src/Logger.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Logger.o src/Logger.cpp

${OBJECTDIR}/src/LogicalNotBranch.o: src/LogicalNotBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/LogicalNotBranch.o src/LogicalNotBranch.cpp

${OBJECTDIR}/src/MacroDefineBranch.o: src/MacroDefineBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MacroDefineBranch.o src/MacroDefineBranch.cpp

${OBJECTDIR}/src/MacroDefinitionIdentifierBranch.o: src/MacroDefinitionIdentifierBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MacroDefinitionIdentifierBranch.o src/MacroDefinitionIdentifierBranch.cpp

${OBJECTDIR}/src/MacroFuncCallBranch.o: src/MacroFuncCallBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MacroFuncCallBranch.o src/MacroFuncCallBranch.cpp

${OBJECTDIR}/src/MacroIfDefBranch.o: src/MacroIfDefBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MacroIfDefBranch.o src/MacroIfDefBranch.cpp

${OBJECTDIR}/src/PTRBranch.o: src/PTRBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PTRBranch.o src/PTRBranch.cpp

${OBJECTDIR}/src/Parser.o: src/Parser.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Parser.o src/Parser.cpp

${OBJECTDIR}/src/Preprocessor.o: src/Preprocessor.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Preprocessor.o src/Preprocessor.cpp

${OBJECTDIR}/src/ReturnBranch.o: src/ReturnBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ReturnBranch.o src/ReturnBranch.cpp

${OBJECTDIR}/src/RootBranch.o: src/RootBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/RootBranch.o src/RootBranch.cpp

${OBJECTDIR}/src/STRUCTAccessBranch.o: src/STRUCTAccessBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STRUCTAccessBranch.o src/STRUCTAccessBranch.cpp

${OBJECTDIR}/src/STRUCTBranch.o: src/STRUCTBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STRUCTBranch.o src/STRUCTBranch.cpp

${OBJECTDIR}/src/STRUCTDEFBranch.o: src/STRUCTDEFBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STRUCTDEFBranch.o src/STRUCTDEFBranch.cpp

${OBJECTDIR}/src/STRUCTDescriptorBranch.o: src/STRUCTDescriptorBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STRUCTDescriptorBranch.o src/STRUCTDescriptorBranch.cpp

${OBJECTDIR}/src/Scope.o: src/Scope.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Scope.o src/Scope.cpp

${OBJECTDIR}/src/ScopeBranch.o: src/ScopeBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ScopeBranch.o src/ScopeBranch.cpp

${OBJECTDIR}/src/SemanticValidator.o: src/SemanticValidator.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SemanticValidator.o src/SemanticValidator.cpp

${OBJECTDIR}/src/StandardScopeBranch.o: src/StandardScopeBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/StandardScopeBranch.o src/StandardScopeBranch.cpp

${OBJECTDIR}/src/Stream.o: src/Stream.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Stream.o src/Stream.cpp

${OBJECTDIR}/src/Token.o: src/Token.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Token.o src/Token.cpp

${OBJECTDIR}/src/Tree.o: src/Tree.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tree.o src/Tree.cpp

${OBJECTDIR}/src/TreeImprover.o: src/TreeImprover.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TreeImprover.o src/TreeImprover.cpp

${OBJECTDIR}/src/VDEFBranch.o: src/VDEFBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/VDEFBranch.o src/VDEFBranch.cpp

${OBJECTDIR}/src/VarIdentifierBranch.o: src/VarIdentifierBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/VarIdentifierBranch.o src/VarIdentifierBranch.cpp

${OBJECTDIR}/src/VirtualObjectFormat.o: src/VirtualObjectFormat.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/VirtualObjectFormat.o src/VirtualObjectFormat.cpp

${OBJECTDIR}/src/VirtualSegment.o: src/VirtualSegment.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/VirtualSegment.o src/VirtualSegment.cpp

${OBJECTDIR}/src/WhileBranch.o: src/WhileBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/WhileBranch.o src/WhileBranch.cpp

${OBJECTDIR}/src/common.o: src/common.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/common.o src/common.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
