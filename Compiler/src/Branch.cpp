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

void Branch::addChild(std::shared_ptr<Branch> branch, std::shared_ptr<Branch> child_to_place_ahead_of)
{
    if (branch == NULL)
    {
        throw Exception("Branch::addChild(std::shared_ptr<Branch> branch): NULL children are not allowed!");
    }

    if (branch->hasParent())
    {
        throw Exception("Branch::addChild(std::shared_ptr<Branch> branch): child already has a parent! Try cloning the branch first");
    }

    try
    {
        branch->validate_identity_on_tree();
    }
    catch (Exception ex)
    {
        ex.setFunctionName("void Branch::addChild(std::shared_ptr<Branch> branch, std::shared_ptr<Branch> child_to_place_ahead_of)");
        throw ex;
    }

    // Lets let this child know who its parent is
    branch->setParent(this->getptr());

    // Set removed flag to false incase this is a previously used branch
    branch->setRemoved(false);

    // Setup local scopes if they are not already set for this child
    if (!branch->hasLocalScope())
    {
        std::shared_ptr<ScopeBranch> scope_branch = std::dynamic_pointer_cast<ScopeBranch>(this->getptr());
        if (scope_branch != NULL)
        {
            // Ok we ourselves are a scope branch so lets set the scope to us
            branch->setLocalScope(scope_branch);
        }
        else
        {
            // We are not a scope branch so lets set the scope for this branch to our local scope
            branch->setLocalScope(getLocalScope());
        }
    }


    if (!branch->hasRootScope())
    {
        branch->setRootScope(getRootScope());
    }

    if (branch->getRoot() != NULL)
    {
        branch->setRoot(getRoot());
    }

    if (child_to_place_ahead_of != NULL)
    {
        // We need to place this new child directly ahead of another one
        this->children.insert(this->children.begin() + getChildPosition(child_to_place_ahead_of), branch);
    }
    else
    {
        this->children.push_back(branch);
    }
}

void Branch::replaceChild(std::shared_ptr<Branch> child, std::shared_ptr<Branch> new_branch)
{
    try
    {
        validate_identity_on_tree();
    }
    catch (Exception ex)
    {
        ex.setFunctionName("void Branch::replaceChild(std::shared_ptr<Branch> child, std::shared_ptr<Branch> new_branch)");
        throw ex;
    }

    try
    {
        new_branch->validate_identity_on_tree();
    }
    catch (Exception ex)
    {
        ex.setFunctionName("void Branch::replaceChild(std::shared_ptr<Branch> child, std::shared_ptr<Branch> new_branch)");
        ex.appendMessage("cannot replace \"child\" with \"new_branch\" as \"new_branch\" has previously been removed or replace from the tree. More info: ", STRING_APPEND_START);
        throw ex;
    }

    for (int i = 0; i < this->children.size(); i++)
    {
        std::shared_ptr<Branch> c = this->children.at(i);
        if (c == child)
        {
            this->children[i] = new_branch;
            new_branch->setParent(this->getptr());
            new_branch->setLocalScope(child->getLocalScope());
            new_branch->setRootScope(child->getRootScope());
            new_branch->setRoot(child->getRoot());
            child->setReplaced(new_branch);
            child->setParent(NULL);
        }
    }
}

void Branch::replaceSelf(std::shared_ptr<Branch> replacee_branch)
{
    try
    {
        validate_identity_on_tree();
    }
    catch (Exception ex)
    {
        ex.setFunctionName("void Branch::replaceSelf(std::shared_ptr<Branch> replacee_branch)");
        throw ex;
    }

    getParent()->replaceChild(this->getptr(), replacee_branch);
}

void Branch::replaceWithChildren()
{
    try
    {
        validate_identity_on_tree();
    }
    catch (Exception ex)
    {
        ex.setFunctionName("void Branch::replaceWithChildren()");
        throw ex;
    }

    if (!hasParent())
    {
        throw Exception("void Branch::replaceWithChildren(): you must have a parent to replace yourself with your children");
    }

    // Only some branches are legal for this
    std::shared_ptr<Branch> parent = getParent();
    std::string type = parent->getType();
    if (type != "root" && type != "BODY")
    {
        throw Exception("void Branch::replaceWithChildren(): parent type is not legal for this operation, only root branch and BODY branch are legal.");
    }

    // Loop through and add the children
    for (std::shared_ptr<Branch> child : getChildren())
    {
        // Remove the our child as children cannot have multiple parents
        child->removeSelf();
        // Add the child to our parent
        parent->addChild(child, this->getptr());
    }

    // Finally remove ourself
    removeSelf();
}

void Branch::removeChild(std::shared_ptr<Branch> child)
{
    for (int i = 0; i < this->children.size(); i++)
    {
        std::shared_ptr<Branch> c = this->children.at(i);
        if (c == child)
        {
            child->setRemoved(true);
            child->setParent(NULL);
            child->setLocalScope(NULL);
            child->setRootScope(NULL);
            child->setRoot(NULL);
            this->children.erase(this->children.begin() + i);
        }
    }
}

void Branch::removeSelf()
{
    try
    {
        validate_identity_on_tree();
    }
    catch (Exception ex)
    {
        ex.setFunctionName("void Branch::removeSelf():");
        throw ex;
    }
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
    try
    {
        validate_identity_on_tree();
    }
    catch (Exception ex)
    {
        ex.setFunctionName("std::shared_ptr<Branch> Branch::getParent()");
        throw ex;
    }
    return this->parent;
}

bool Branch::hasParent()
{
    return this->parent != NULL;
}

std::shared_ptr<Branch> Branch::getFirstChildOfType(std::string type)
{
    for (std::shared_ptr<Branch> child : getChildren())
    {
        if (child->getType() == type)
            return child;
    };

    throw Exception("std::shared_ptr<Branch> Branch::getFirstChildOfType(std::string type): child not found");
}

bool Branch::hasChildOfType(std::string type)
{
    for (std::shared_ptr<Branch> child : getChildren())
    {
        if (child->getType() == type)
            return true;
    };

    return false;
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

bool Branch::hasLocalScope()
{
    return getLocalScope() != NULL;
}

bool Branch::hasRootScope()
{
    return getRootScope() != NULL;
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

int Branch::getChildPosition(std::shared_ptr<Branch> child)
{
    int pos = 0;
    for (std::shared_ptr<Branch> c : getChildren())
    {
        if (c == child)
            return pos;
        pos++;
    }

    return pos;
}

std::shared_ptr<Branch> Branch::getReplaceeBranch()
{
    return this->replacee_branch;
}

int Branch::getBranchType()
{
    return BRANCH_TYPE_BRANCH;
}

void Branch::validate_identity_on_tree()
{
    if (this->wasReplaced())
    {
        throw Exception(""
                        "this branch of type \"" + getType() + "\" has been replaced with branch of type \"" + getReplaceeBranch()->getType() + "\". Please use the replacee branch.",
                        "void Branch::validate_identity_on_tree()");
    }

    if (isRemoved())
    {
        throw Exception("this branch of type \"" + getType() + "\" has been flagged as removed",
                        "void Branch::validate_identity_on_tree()");
    }
}

void Branch::validity_check()
{
    validate_identity_on_tree();
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

    cloned_branch->setLocalScope(getLocalScope());
    cloned_branch->setRootScope(getRootScope());
    cloned_branch->setRoot(getRoot());
    return cloned_branch;
}