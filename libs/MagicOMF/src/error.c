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
 * File:   common.c
 * Author: Daniel McCarthy
 *
 * Created on 06 December 2016, 11:59
 */

#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "IO.h"

const char* error_messages[] = 
{
    "The THEADR to process is invalid.",
    "Expecting a THEADR or an LHEADR but none was provided.",
    "The LHEADR to process is invalid",
    "An invalid record type was provided",
    "An invalid COMENT record was provided",
    "An unimplemented or illegal COMENT class was provided",
    "An invalid LNAME record was provided",
    "An invalid SEGDEF_16 record was provided",
    "The SEGDEF_16 record provided is not formatted correctly",
    "An invalid PUBDEF-16 record was provided",
    "No support for absolute addressing for PUBDEF-16 record, please use a different linker.",
    "An invalid LEDATA record was provided",
    "An invalid FIXUPP-16 record was provided",
    "THREAD's in FIXUPP-16 records are not supported by this linker",
    "The location selected for the FIXUPP-16 is not supported",
    "The options in the fix data byte for the FIXUPP-16 record are not supported",
    "The MODEND-16 record to process is invalid",
    "The module type for this MODEND-16 record is not supported",
    "The record associated its self with an LNAMES record that did not exist.",
    "The OMF file does not appear to be properly formatted",
    "The EXTDEF to process is invalid.",
    "The offset for the FIXUPP-16 record is out of bounds"
};

void error(MAGIC_OMF_ERROR_CODE error_code, struct MagicOMFHandle* handle)
{
    handle->has_error = true;
    handle->last_error_code = error_code;
}

const char* GetErrorMessage(MAGIC_OMF_ERROR_CODE error_code)
{
    return error_messages[error_code];
}