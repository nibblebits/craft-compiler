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

#ifndef GOBLINLIBRARYLOADER_H
#define GOBLINLIBRARYLOADER_H

#define EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32
    #error Goblin Library Loader is only compatible with windows at the moment
#endif

    void* EXPORT GoblinLoadLibrary(const char* filename);
    void* EXPORT GoblinGetAddress(void* library, const char* entity_name);

#ifdef __cplusplus
}
#endif

#endif /* GOBLINLIBRARYLOADER_H */

