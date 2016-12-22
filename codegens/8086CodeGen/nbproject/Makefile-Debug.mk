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
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/Assembler8086.o \
	${OBJECTDIR}/src/CodeGen8086.o \
	${OBJECTDIR}/src/InstructionBranch.o \
	${OBJECTDIR}/src/LabelBranch.o \
	${OBJECTDIR}/src/OperandBranch.o \
	${OBJECTDIR}/src/SegmentBranch.o


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
LDLIBSOPTIONS=../../bin/Compiler.dll

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../bin/codegens/8086CodeGen.${CND_DLIB_EXT}

../../bin/codegens/8086CodeGen.${CND_DLIB_EXT}: ../../bin/Compiler.dll

../../bin/codegens/8086CodeGen.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ../../bin/codegens
	${LINK.cc} -o ../../bin/codegens/8086CodeGen.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/Assembler8086.o: src/Assembler8086.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Assembler8086.o src/Assembler8086.cpp

${OBJECTDIR}/src/CodeGen8086.o: src/CodeGen8086.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CodeGen8086.o src/CodeGen8086.cpp

${OBJECTDIR}/src/InstructionBranch.o: src/InstructionBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/InstructionBranch.o src/InstructionBranch.cpp

${OBJECTDIR}/src/LabelBranch.o: src/LabelBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/LabelBranch.o src/LabelBranch.cpp

${OBJECTDIR}/src/OperandBranch.o: src/OperandBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/OperandBranch.o src/OperandBranch.cpp

${OBJECTDIR}/src/SegmentBranch.o: src/SegmentBranch.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SegmentBranch.o src/SegmentBranch.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} -r ../../bin/codegens/Compiler.dll
	${RM} ../../bin/codegens/8086CodeGen.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
