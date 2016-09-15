/*
    Goblin Library Loader v1.0 - A loader for loading dynamic libraries for Windows and Linux
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

#include "GoblinLibraryLoader.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>

void* EXPORT GoblinLoadLibrary(const char* filename) {
    void* lib_ptr = 0;
#ifdef _WIN32
    lib_ptr = LoadLibrary(filename);
#else
#error "not yet supported in linux"
#endif

    return lib_ptr;
}

void* EXPORT GoblinGetAddress(void* library, const char* entryPoint) {
#ifdef _WIN32
    return (void*) GetProcAddress(library, entryPoint);
#else
#error "not yet supported in linux"
#endif
}
