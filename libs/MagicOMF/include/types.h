/*
    Magic OMF Library v1.0 - A linker for the OMF(Object Model Format).
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
 * File:   types.h
 * Author: Daniel McCarthy
 *
 * Created on 06 December 2016, 11:09
 * 
 * Description: Simple types to represent in a more readable fashion.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

#ifndef __cplusplus
typedef unsigned char bool;

#define false 0;
#define true 1;
#endif

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;

typedef uint8 MAGIC_OMF_ERROR_CODE;
typedef unsigned char COMMENT_TYPE;
typedef unsigned char LOCATION_TYPE;


#endif /* TYPES_H */

