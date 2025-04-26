/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 4/22/25.
//

#include "TreeNode.hpp"

#include "Subsystem.hpp"

namespace optifol
{

TreeNode::TreeNode(TreeNode * const parent) :
    children(Gio::ListStore<Subsystem>::create()),
    parent(parent)
{
}

Glib::RefPtr<Gio::ListStore<Subsystem>> TreeNode::get_children()
{
    return children;
}

Glib::RefPtr<const Gio::ListStore<Subsystem>> TreeNode::get_children() const
{
    return children;
}

void TreeNode::add(std::string &&name, const std::chrono::system_clock::time_point &created_time,
    const std::chrono::system_clock::time_point &last_modified_time)
{
    children->append(Glib::make_refptr_for_instance(new Subsystem(std::move(name), created_time, last_modified_time,
        this)));
}

const TreeNode * TreeNode::get_parent() const
{
    return parent;
}

TreeNode * TreeNode::get_parent()
{
    return parent;
}

}
