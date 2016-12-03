/*
    Goblin compiler v1.0 - The standard compiler for the Goblin language.
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
 * File:   BODYBranch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 19 October 2016, 16:04
 * 
 * Description: 
 */

#include "BODYBranch.h"
#include "VDEFBranch.h"
#include "FORBranch.h"

BODYBranch::BODYBranch(Compiler* compiler) : ScopeBranch(compiler, "BODY", "")
{
}

BODYBranch::~BODYBranch()
{
}

int BODYBranch::getScopeSize(bool include_subscopes, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_start, std::function<bool(std::shared_ptr<Branch> child_branch) > child_proc_end, bool *should_stop)
{
    int size = 0;
    for (std::shared_ptr<Branch> child : this->getChildren())
    {
        if (child_proc_start != NULL)
        {
            if (!child_proc_start(child))
            {
                if (should_stop != NULL)
                {
                    *should_stop = true;
                }
                break;
            }
        }

        std::string child_type = child->getType();
        if (child_type == "V_DEF" ||
                child_type == "STRUCT_DEF")
        {
            std::shared_ptr<VDEFBranch> vdef_branch = std::dynamic_pointer_cast<VDEFBranch>(child);
            size += vdef_branch->getDataTypeSize();
        }
        else if (include_subscopes)
        {
            if (child_type == "FOR")
            {
                std::shared_ptr<FORBranch> for_branch = std::dynamic_pointer_cast<FORBranch>(child);
                size += for_branch->getScopeSize(include_subscopes, child_proc_start, child_proc_end, should_stop);
                if (*should_stop)
                {
                    break;
                }
            }
        }

        if (child_proc_end != NULL)
        {
            if (!child_proc_end(child))
            {
                if (should_stop != NULL)
                {
                    *should_stop = true;
                }
                break;
            }
        }
    }
    return size;
}