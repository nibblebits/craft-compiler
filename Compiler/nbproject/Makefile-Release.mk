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
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/ASMBranch.o \
	${OBJECTDIR}/src/ASTAssistant.o \
	${OBJECTDIR}/src/AddressOfBranch.o \
	${OBJECTDIR}/src/ArrayBranch.o \
	${OBJECTDIR}/src/ArrayIndexBranch.o \
	${OBJECTDIR}/src/AssignBranch.o \
	${OBJECTDIR}/src/BODYBranch.o \
	${OBJECTDIR}/src/Branch.o \
	${OBJECTDIR}/src/CallBranch.o \
	${OBJECTDIR}/src/CodeGenerator.o \
	${OBJECTDIR}/src/Compiler.o \
	${OBJECTDIR}/src/CompilerEntity.o \
	${OBJECTDIR}/src/CustomBranch.o \
	${OBJECTDIR}/src/EBranch.o \
	${OBJECTDIR}/src/ELSEBranch.o \
	${OBJECTDIR}/src/FORBranch.o \
	${OBJECTDIR}/src/FuncBranch.o \
	${OBJECTDIR}/src/FuncCallBranch.o \
	${OBJECTDIR}/src/GoblinByteCodeGenerator.o \
	${OBJECTDIR}/src/GoblinByteCodeLinker.o \
	${OBJECTDIR}/src/GoblinObject.o \
	${OBJECTDIR}/src/GoblinObjectLinker.o \
	${OBJECTDIR}/src/Helper.o \
	${OBJECTDIR}/src/IFBranch.o \
	${OBJECTDIR}/src/Lexer.o \
	${OBJECTDIR}/src/Linker.o \
	${OBJECTDIR}/src/Logger.o \
	${OBJECTDIR}/src/MathEBranch.o \
	${OBJECTDIR}/src/PTRAssignBranch.o \
	${OBJECTDIR}/src/PTRBranch.o \
	${OBJECTDIR}/src/Parser.o \
	${OBJECTDIR}/src/ParserRule.o \
	${OBJECTDIR}/src/ParserRuleRequirement.o \
	${OBJECTDIR}/src/STRUCTAccessBranch.o \
	${OBJECTDIR}/src/STRUCTBranch.o \
	${OBJECTDIR}/src/STRUCTDEFBranch.o \
	${OBJECTDIR}/src/Scope.o \
	${OBJECTDIR}/src/Stream.o \
	${OBJECTDIR}/src/Token.o \
	${OBJECTDIR}/src/Tree.o \
	${OBJECTDIR}/src/TypeChecker.o \
	${OBJECTDIR}/src/VDEFBranch.o \
	${OBJECTDIR}/src/VarIdentifierBranch.o \
	${OBJECTDIR}/src/common.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/compiler.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/compiler.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/compiler ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/ASMBranch.o: src/ASMBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ASMBranch.o src/ASMBranch.cpp

${OBJECTDIR}/src/ASTAssistant.o: src/ASTAssistant.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ASTAssistant.o src/ASTAssistant.cpp

${OBJECTDIR}/src/AddressOfBranch.o: src/AddressOfBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AddressOfBranch.o src/AddressOfBranch.cpp

${OBJECTDIR}/src/ArrayBranch.o: src/ArrayBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ArrayBranch.o src/ArrayBranch.cpp

${OBJECTDIR}/src/ArrayIndexBranch.o: src/ArrayIndexBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ArrayIndexBranch.o src/ArrayIndexBranch.cpp

${OBJECTDIR}/src/AssignBranch.o: src/AssignBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AssignBranch.o src/AssignBranch.cpp

${OBJECTDIR}/src/BODYBranch.o: src/BODYBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BODYBranch.o src/BODYBranch.cpp

${OBJECTDIR}/src/Branch.o: src/Branch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Branch.o src/Branch.cpp

${OBJECTDIR}/src/CallBranch.o: src/CallBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CallBranch.o src/CallBranch.cpp

${OBJECTDIR}/src/CodeGenerator.o: src/CodeGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CodeGenerator.o src/CodeGenerator.cpp

${OBJECTDIR}/src/Compiler.o: src/Compiler.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Compiler.o src/Compiler.cpp

${OBJECTDIR}/src/CompilerEntity.o: src/CompilerEntity.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CompilerEntity.o src/CompilerEntity.cpp

${OBJECTDIR}/src/CustomBranch.o: src/CustomBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CustomBranch.o src/CustomBranch.cpp

${OBJECTDIR}/src/EBranch.o: src/EBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/EBranch.o src/EBranch.cpp

${OBJECTDIR}/src/ELSEBranch.o: src/ELSEBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ELSEBranch.o src/ELSEBranch.cpp

${OBJECTDIR}/src/FORBranch.o: src/FORBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FORBranch.o src/FORBranch.cpp

${OBJECTDIR}/src/FuncBranch.o: src/FuncBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FuncBranch.o src/FuncBranch.cpp

${OBJECTDIR}/src/FuncCallBranch.o: src/FuncCallBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FuncCallBranch.o src/FuncCallBranch.cpp

${OBJECTDIR}/src/GoblinByteCodeGenerator.o: src/GoblinByteCodeGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GoblinByteCodeGenerator.o src/GoblinByteCodeGenerator.cpp

${OBJECTDIR}/src/GoblinByteCodeLinker.o: src/GoblinByteCodeLinker.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GoblinByteCodeLinker.o src/GoblinByteCodeLinker.cpp

${OBJECTDIR}/src/GoblinObject.o: src/GoblinObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GoblinObject.o src/GoblinObject.cpp

${OBJECTDIR}/src/GoblinObjectLinker.o: src/GoblinObjectLinker.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GoblinObjectLinker.o src/GoblinObjectLinker.cpp

${OBJECTDIR}/src/Helper.o: src/Helper.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Helper.o src/Helper.cpp

${OBJECTDIR}/src/IFBranch.o: src/IFBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/IFBranch.o src/IFBranch.cpp

${OBJECTDIR}/src/Lexer.o: src/Lexer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Lexer.o src/Lexer.cpp

${OBJECTDIR}/src/Linker.o: src/Linker.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Linker.o src/Linker.cpp

${OBJECTDIR}/src/Logger.o: src/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Logger.o src/Logger.cpp

${OBJECTDIR}/src/MathEBranch.o: src/MathEBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MathEBranch.o src/MathEBranch.cpp

${OBJECTDIR}/src/PTRAssignBranch.o: src/PTRAssignBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PTRAssignBranch.o src/PTRAssignBranch.cpp

${OBJECTDIR}/src/PTRBranch.o: src/PTRBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PTRBranch.o src/PTRBranch.cpp

${OBJECTDIR}/src/Parser.o: src/Parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Parser.o src/Parser.cpp

${OBJECTDIR}/src/ParserRule.o: src/ParserRule.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ParserRule.o src/ParserRule.cpp

${OBJECTDIR}/src/ParserRuleRequirement.o: src/ParserRuleRequirement.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ParserRuleRequirement.o src/ParserRuleRequirement.cpp

${OBJECTDIR}/src/STRUCTAccessBranch.o: src/STRUCTAccessBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STRUCTAccessBranch.o src/STRUCTAccessBranch.cpp

${OBJECTDIR}/src/STRUCTBranch.o: src/STRUCTBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STRUCTBranch.o src/STRUCTBranch.cpp

${OBJECTDIR}/src/STRUCTDEFBranch.o: src/STRUCTDEFBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STRUCTDEFBranch.o src/STRUCTDEFBranch.cpp

${OBJECTDIR}/src/Scope.o: src/Scope.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Scope.o src/Scope.cpp

${OBJECTDIR}/src/Stream.o: src/Stream.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Stream.o src/Stream.cpp

${OBJECTDIR}/src/Token.o: src/Token.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Token.o src/Token.cpp

${OBJECTDIR}/src/Tree.o: src/Tree.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tree.o src/Tree.cpp

${OBJECTDIR}/src/TypeChecker.o: src/TypeChecker.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TypeChecker.o src/TypeChecker.cpp

${OBJECTDIR}/src/VDEFBranch.o: src/VDEFBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/VDEFBranch.o src/VDEFBranch.cpp

${OBJECTDIR}/src/VarIdentifierBranch.o: src/VarIdentifierBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/VarIdentifierBranch.o src/VarIdentifierBranch.cpp

${OBJECTDIR}/src/common.o: src/common.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/common.o src/common.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/compiler.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
