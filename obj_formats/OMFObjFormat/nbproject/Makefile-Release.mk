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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/OMFObjectFormat.o \
	${OBJECTDIR}/src/main.o


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
LDLIBSOPTIONS=../../bin/MagicOMF.dll ../../bin/Compiler.dll

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../bin/obj_formats/omf.${CND_DLIB_EXT}

../../bin/obj_formats/omf.${CND_DLIB_EXT}: ../../bin/MagicOMF.dll

../../bin/obj_formats/omf.${CND_DLIB_EXT}: ../../bin/Compiler.dll

../../bin/obj_formats/omf.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ../../bin/obj_formats
	${LINK.cc} -o ../../bin/obj_formats/omf.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared

${OBJECTDIR}/src/OMFObjectFormat.o: src/OMFObjectFormat.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DRELEASE -Iinclude -I../../libs/MagicOMF/include -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/OMFObjectFormat.o src/OMFObjectFormat.cpp

${OBJECTDIR}/src/main.o: src/main.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DRELEASE -Iinclude -I../../libs/MagicOMF/include -I../../Compiler/include -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} -r ../../bin/obj_formats/MagicOMF.dll ../../bin/obj_formats/Compiler.dll
	${RM} ../../bin/obj_formats/omf.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
