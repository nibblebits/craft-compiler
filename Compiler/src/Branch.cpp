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
 * File:   Branch.cpp
 * Author: Daniel McCarthy
 *
 * Created on 29 May 2016, 20:41
 * 
 * Description: This is a branch and it also holds its parent and child branches.
 */

#include "Branch.h"
#include "ScopeBranch.h"
#include "RootBranch.h"

Branch::Branch(std::string type, std::string value)
{
    this->type = type;
    this->value = value;
    this->parent = NULL;
    this->is_removed = false;
    this->replacee_branch = NULL;
    this->root_branch = NULL;
    this->root_scope = NULL;
    this->local_scope = NULL;
}

Branch::~Branch()
{

}

void Branch::addChild(std::shared_ptr<Branch> branch)
{
    if (branch == NULL)
    {
        throw Exception("Branch::addChild(std::shared_ptr<Branch> branch): NULL children are not allowed!");
    }

    // Lets let this child know who its parent is
    branch->setParent(this->getptr());
    this->children.push_back(branch);
}

void Branch::replaceChild(std::shared_ptr<Branch> child, std::shared_ptr<Branch> new_branch)
{
    for (int i = 0; i < this->children.size(); i++)
    {
        std::shared_ptr<Branch> c = this->children.at(i);
        if (c == child)
        {
            this->children[i] = new_branch;
            new_branch->setParent(this->getptr());
            child->setReplaced(new_branch);
        }
    }
}

void Branch::replaceSelf(std::shared_ptr<Branch> replacee_branch)
{
    getParent()->replaceChild(this->getptr(), replacee_branch);
}

void Branch::removeChild(std::shared_ptr<Branch> child)
{
    for (int i = 0; i < this->children.size(); i++)
    {
        std::shared_ptr<Branch> c = this->children.at(i);
        if (c == child)
        {
            child->setRemoved(true);
            this->children.erase(this->children.begin() + i);
        }
    }
}

void Branch::removeSelf()
{
    getParent()->removeChild(this->getptr());
}

void Branch::setRemoved(bool is_removed)
{
    this->is_removed = is_removed;
}

void Branch::setReplaced(std::shared_ptr<Branch> replacee_branch)
{
    setRemoved(true);
    this->replacee_branch = replacee_branch;
}

void Branch::iterate_children(std::function<void(std::shared_ptr<Branch> child_branch) > iterate_func)
{
    for (std::shared_ptr<Branch> child_branch : this->getChildren())
    {
        iterate_func(child_branch);
    }
}

bool Branch::hasChild(std::shared_ptr<Branch> branch)
{
    for (int i = 0; i < this->children.size(); i++)
    {
        std::shared_ptr<Branch> c = this->children.at(i);
        if (c == branch)
        {
            return true;
        }
    }

    return false;
}

bool Branch::hasChildren()
{
    return this->children.size() > 0;
}

void Branch::setParent(std::shared_ptr<Branch> branch)
{
    this->parent = branch;
}

void Branch::setValue(std::string value)
{
    this->value = value;
}

void Branch::setRoot(std::shared_ptr<RootBranch> root_branch)
{
    this->root_branch = root_branch;
}

void Branch::setRootScope(std::shared_ptr<ScopeBranch> root_scope, bool set_to_all_children)
{
    if (local_scope == this->getptr())
        throw Exception("Branch::setRootScope(std::shared_ptr<ScopeBranch> root_scope): attempting to set scope to self");

    this->root_scope = root_scope;

    if (set_to_all_children)
    {
        // Ok we should set the scope to all children, and they will set it to all their children
        for (std::shared_ptr<Branch> child : this->getChildren())
        {
            child->setRootScope(root_scope, true);
        }
    }
}

void Branch::setLocalScope(std::shared_ptr<ScopeBranch> local_scope, bool set_to_all_children)
{
    if (local_scope == this->getptr())
        throw Exception("Branch::setLocalScope(std::shared_ptr<ScopeBranch> local_scope): attempting to set scope to self");

    this->local_scope = local_scope;

    if (set_to_all_children)
    {
        // Ok we should set the scope to all children, and they will set it to all their children
        for (std::shared_ptr<Branch> child : this->getChildren())
        {
            child->setLocalScope(local_scope, true);
        }
    }

}

std::shared_ptr<Branch> Branch::getFirstChild()
{
    return this->getChildren()[0];
}

std::shared_ptr<Branch> Branch::getSecondChild()
{
    return this->getChildren()[1];
}

std::shared_ptr<Branch> Branch::getThirdChild()
{
    return this->getChildren()[2];
}

std::shared_ptr<Branch> Branch::getFourthChild()
{
    return this->getChildren()[3];
}

std::vector<std::shared_ptr<Branch>> Branch::getChildren()
{
    return this->children;
}

std::shared_ptr<Branch> Branch::getParent()
{
    return this->parent;
}

bool Branch::hasParent()
{
    return this->parent != NULL;
}

std::shared_ptr<Branch> Branch::lookUpTreeUntilParentTypeFound(std::string parent_type_to_find)
{
    if (this->parent == NULL)
    {
        // Not found
        return NULL;
    }

    if (this->parent->getType() == parent_type_to_find)
    {
        return this->parent;
    }
    else
    {
        return this->parent->lookUpTreeUntilParentTypeFound(parent_type_to_find);
    }
}

std::shared_ptr<Branch> Branch::lookDownTreeUntilFirstChildOfType(std::string type)
{
    for (std::shared_ptr<Branch> child : this->getChildren())
    {
        if (child->getType() == type)
        {
            return child;
        }

        child = child->lookDownTreeUntilFirstChildOfType(type);
        return child;
    }

    return NULL;
}

std::shared_ptr<Branch> Branch::lookDownTreeUntilLastChildOfType(std::string type)
{
    std::shared_ptr<Branch> branch = this->getptr();
    std::shared_ptr<Branch> last_valid_branch = NULL;
    do
    {
        branch = branch->lookDownTreeUntilFirstChildOfType(type);
        if (branch != NULL)
            last_valid_branch = branch;
    }
    while (branch != NULL);

    return last_valid_branch;
}

std::string Branch::getType()
{
    return this->type;
}

std::string Branch::getValue()
{
    return this->value;
}

std::shared_ptr<ScopeBranch> Branch::getRootScope()
{
    return this->root_scope;
}

std::shared_ptr<ScopeBranch> Branch::getLocalScope()
{
    return this->local_scope;
}

std::shared_ptr<RootBranch> Branch::getRoot()
{
    return this->root_branch;
}

std::shared_ptr<Branch> Branch::getptr()
{
    return shared_from_this();
}

bool Branch::wasReplaced()
{
    return this->replacee_branch != NULL;
}

bool Branch::isRemoved()
{
    return this->is_removed;
}

std::shared_ptr<Branch> Branch::getReplaceeBranch()
{
    return this->replacee_branch;
}

int Branch::getBranchType()
{
    return BRANCH_TYPE_BRANCH;
}

void Branch::validity_check()
{
    // Nothing to validate.
}

void Branch::rebuild()
{
    throw Exception("void Branch::rebuild(): This branch of type \"" + getType() + "\" does not support rebuilding.");
}

// Scopes must not be cloned.

std::shared_ptr<Branch> Branch::clone()
{
    // This is not a custom branch, just clone our children
    std::shared_ptr<Branch> cloned_branch = std::shared_ptr<Branch>(new Branch(getType(), getValue()));
    for (std::shared_ptr<Branch> child : Branch::getChildren())
    {
        cloned_branch->addChild(child->clone());
    }
    return cloned_branch;
}