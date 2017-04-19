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
#include <string.h>

void str_replace(char* store, const char* str, const char* find, const char* replace)
{
    int str_len = strlen(str);
    int find_len = strlen(find);
    int replace_len = strlen(replace);
    
    int si = 0;
    int i = 0;
    for (i = 0; i < str_len; i++)
    {
        if (str[i] == find[0])
        {
            int fp = 0;
            for (fp = 0; fp < find_len; fp++)
            {
                int index = i+fp;
                if (str[index] != find[fp])
                {
                    break;
                }
            }
            
            // Did we have a match
            if (fp == find_len)
            {
                // Ok then lets add the replacement
                for (int rp = 0; rp < replace_len; rp++)
                {
                    store[si] = replace[rp];
                    si++;
                }
                i += find_len-1;
            }
        }
        else
        {
            store[si] = str[i];
            si++;
        }
    }
    
    // Null terminator
    store[si] = 0;
}

void handle_values(char* store, const char* str)
{
    char path[512];
    char buf[512];
    GetModuleFileName(NULL, buf, 512);
    str_replace(path, buf, "\\", "/");
    *(strrchr(path, '/')) = 0;
    str_replace(store, str,  "%exe_dir", path);   
}
bool EXPORT GoblinLoadDependencies(const char* def_filename)
{
    FILE* fp;
    char* line = 0;
    size_t len = 0;
    fp = fopen(def_filename, "r");
    if (!fp) {
        return false;
    }

    while(getline(&line, &len, fp) != -1)
    {
        char handled_line[512];
        handle_values(handled_line, line);
        GoblinLoadLibrary(handled_line);
    }
    
    if (line)
        free(line);
    fclose(fp);
    return true;
}

void* EXPORT GoblinLoadLibrary(const char* filename)
{
    void* lib_ptr = 0;
#ifdef _WIN32
    
    char dep_filename[strlen(filename+4)];
    strcpy(dep_filename, filename);
    strcat(dep_filename, ".dep");
    
    // Load dependencies of this library
    GoblinLoadDependencies(dep_filename);
    
    lib_ptr = LoadLibrary(filename);
#else
#error "not yet supported in linux"
#endif

    return lib_ptr;
}

void* EXPORT GoblinGetAddress(void* library, const char* entryPoint)
{
#ifdef _WIN32
    return (void*) GetProcAddress(library, entryPoint);
#else
#error "not yet supported in linux"
#endif
}
