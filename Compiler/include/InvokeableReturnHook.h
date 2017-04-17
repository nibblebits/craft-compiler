/*
    Craft compiler v0.1.0 - The standard compiler for the Craft programming language.
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
 * File:   InvokeableReturnHook.h
 * Author: Daniel McCarthy
 *
 * Created on 17 April 2017, 13:21
 */

#ifndef INVOKEABLERETURNHOOK_H
#define INVOKEABLERETURNHOOK_H

#include "Hook.h"

template <typename T>
class InvokeableReturnHook : public Hook
{
public:
    InvokeableReturnHook(INVOKEABLE_RETURNABLE_HOOK_FUNCTION hook_func)
    {
        this->hook_func = hook_func;
    }
    virtual ~InvokeableReturnHook()
    {
        
    }

    T invoke(const char* argument)
    {
        return (T) hook_func(argument);
    }
private:
    INVOKEABLE_RETURNABLE_HOOK_FUNCTION hook_func;
};

#endif /* INVOKEABLERETURNHOOK_H */

