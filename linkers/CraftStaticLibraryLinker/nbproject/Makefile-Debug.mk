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
	${OBJECTDIR}/src/CraftStaticLibraryLinker.o


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
LDLIBSOPTIONS=../../bin/Compiler.dll ../../bin/obj_formats/csl.dll

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../bin/linkers/csl.${CND_DLIB_EXT}

../../bin/linkers/csl.${CND_DLIB_EXT}: ../../bin/Compiler.dll

../../bin/linkers/csl.${CND_DLIB_EXT}: ../../bin/obj_formats/csl.dll

../../bin/linkers/csl.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ../../bin/linkers
	${LINK.cc} -o ../../bin/linkers/csl.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG_MODE -Iinclude -I../../obj_formats/CraftStaticLibrary/include -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/CraftStaticLibraryLinker.o: src/CraftStaticLibraryLinker.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG_MODE -Iinclude -I../../obj_formats/CraftStaticLibrary/include -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CraftStaticLibraryLinker.o src/CraftStaticLibraryLinker.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} -r ../../bin/linkers/Compiler.dll ../../bin/linkers/csl.dll
	${RM} ../../bin/linkers/csl.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
