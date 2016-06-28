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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/ASTAssistant.o \
	${OBJECTDIR}/src/AssignBranch.o \
	${OBJECTDIR}/src/Branch.o \
	${OBJECTDIR}/src/CallBranch.o \
	${OBJECTDIR}/src/CodeGenerator.o \
	${OBJECTDIR}/src/Compiler.o \
	${OBJECTDIR}/src/CompilerEntity.o \
	${OBJECTDIR}/src/CustomBranch.o \
	${OBJECTDIR}/src/EBranch.o \
	${OBJECTDIR}/src/FuncBranch.o \
	${OBJECTDIR}/src/GoblinByteCodeGenerator.o \
	${OBJECTDIR}/src/Helper.o \
	${OBJECTDIR}/src/Lexer.o \
	${OBJECTDIR}/src/MathEBranch.o \
	${OBJECTDIR}/src/Parser.o \
	${OBJECTDIR}/src/ParserRule.o \
	${OBJECTDIR}/src/ParserRuleRequirement.o \
	${OBJECTDIR}/src/Stream.o \
	${OBJECTDIR}/src/Token.o \
	${OBJECTDIR}/src/Tree.o \
	${OBJECTDIR}/src/TypeChecker.o \
	${OBJECTDIR}/src/VDEFBranch.o


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
LDLIBSOPTIONS=-LGoblinLibraryLoader/dist/Debug/MinGW-Windows -LGoblinArgumentParser/dist/Debug/MinGW-Windows -lGoblinLibraryLoader -lGoblinArgumentParser

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/goblincompilerv1.0.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/goblincompilerv1.0.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/goblincompilerv1.0 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/ASTAssistant.o: src/ASTAssistant.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ASTAssistant.o src/ASTAssistant.cpp

${OBJECTDIR}/src/AssignBranch.o: src/AssignBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AssignBranch.o src/AssignBranch.cpp

${OBJECTDIR}/src/Branch.o: src/Branch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Branch.o src/Branch.cpp

${OBJECTDIR}/src/CallBranch.o: src/CallBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CallBranch.o src/CallBranch.cpp

${OBJECTDIR}/src/CodeGenerator.o: src/CodeGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CodeGenerator.o src/CodeGenerator.cpp

${OBJECTDIR}/src/Compiler.o: src/Compiler.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Compiler.o src/Compiler.cpp

${OBJECTDIR}/src/CompilerEntity.o: src/CompilerEntity.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CompilerEntity.o src/CompilerEntity.cpp

${OBJECTDIR}/src/CustomBranch.o: src/CustomBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CustomBranch.o src/CustomBranch.cpp

${OBJECTDIR}/src/EBranch.o: src/EBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/EBranch.o src/EBranch.cpp

${OBJECTDIR}/src/FuncBranch.o: src/FuncBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FuncBranch.o src/FuncBranch.cpp

${OBJECTDIR}/src/GoblinByteCodeGenerator.o: src/GoblinByteCodeGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GoblinByteCodeGenerator.o src/GoblinByteCodeGenerator.cpp

${OBJECTDIR}/src/Helper.o: src/Helper.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Helper.o src/Helper.cpp

${OBJECTDIR}/src/Lexer.o: src/Lexer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Lexer.o src/Lexer.cpp

${OBJECTDIR}/src/MathEBranch.o: src/MathEBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MathEBranch.o src/MathEBranch.cpp

${OBJECTDIR}/src/Parser.o: src/Parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Parser.o src/Parser.cpp

${OBJECTDIR}/src/ParserRule.o: src/ParserRule.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ParserRule.o src/ParserRule.cpp

${OBJECTDIR}/src/ParserRuleRequirement.o: src/ParserRuleRequirement.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ParserRuleRequirement.o src/ParserRuleRequirement.cpp

${OBJECTDIR}/src/Stream.o: src/Stream.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Stream.o src/Stream.cpp

${OBJECTDIR}/src/Token.o: src/Token.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Token.o src/Token.cpp

${OBJECTDIR}/src/Tree.o: src/Tree.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tree.o src/Tree.cpp

${OBJECTDIR}/src/TypeChecker.o: src/TypeChecker.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TypeChecker.o src/TypeChecker.cpp

${OBJECTDIR}/src/VDEFBranch.o: src/VDEFBranch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -IGoblinLibraryLoader/include -IGoblinArgumentParser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/VDEFBranch.o src/VDEFBranch.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/goblincompilerv1.0.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
