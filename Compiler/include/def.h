/*
    Craft Compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   def.h
 * Author: Daniel McCarthy
 *
 * Created on 28 May 2016, 19:48
 * 
 * Description: This header contains definitions such as debug mode and other compiler related settings.
 */

#ifndef DEF_H
#define DEF_H

#define COMPILER_NAME "Craft compiler"
#define COMPILER_VERSION "v1.0 beta"
#define COMPILER_FULLNAME COMPILER_NAME " " COMPILER_VERSION

// Uncomment this line to enable debug mode, this will display debug related information related to the compiler
#define DEBUG_MODE
// Only uncomment the below if this is a release
//#define RELEASE


#define CODEGEN_DIR "./codegens"
#define OBJ_FORMAT_DIR "./obj_formats"

// If its a CYGWIN compiler then enable _WIN32
#ifdef __CYGWIN__
#define _WIN32
#endif

#ifdef _WIN32
#define CODEGEN_EXT ".dll"
#else
#define CODEGEN_EXT ".so"
#endif

#ifdef EXTERNAL_USE
#define EXPORT __declspec(dllimport)
#else
#define EXPORT __declspec(dllexport)
#endif


enum
{
    BRANCH_TYPE_BRANCH,
    BRANCH_TYPE_TOKEN,
    BRANCH_TYPE_VDEF
};

typedef unsigned int VARIABLE_TYPE;

enum
{
    VARIABLE_TYPE_UNKNOWN,
    VARIABLE_TYPE_GLOBAL_VARIABLE,
    VARIABLE_TYPE_FUNCTION_VARIABLE,
    VARIABLE_TYPE_FUNCTION_ARGUMENT_VARIABLE
};


// Options for getting variable offsets
enum
{
    POSITION_OPTION_START_WITH_VARSIZE = 0x01,
    POSITION_OPTION_IGNORE_STRUCTURE_ACCESS = 0x02,
    POSITION_OPTION_TREAT_AS_IF_NOT_POINTER = 0x04,
    POSITION_OPTION_STOP_AT_ROOT_VAR = 0x08
};

typedef unsigned int POSITION_OPTIONS;

enum
{
    GET_SCOPE_SIZE_INCLUDE_SUBSCOPES = 0x01,
    GET_SCOPE_SIZE_INCLUDE_PARENT_SCOPES = 0x02
};

typedef unsigned int GET_SCOPE_SIZE_OPTIONS;

enum
{
    FIXUP_8BIT,
    FIXUP_16BIT,
    FIXUP_32BIT,
    FIXUP_64_BIT
};

typedef unsigned int FIXUP_LENGTH;


#endif /* DEF_H */

