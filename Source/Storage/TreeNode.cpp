/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the tree node abstract storage object
 * @author Oliver Dixon
 * @date 2025-04-26
 * @version Development
 */

#include "TreeNode.hpp"

#include "Subsystem.hpp"

namespace optifol
{

TreeNode::TreeNode() :
    children(Gio::ListStore<Subsystem>::create())
{
}

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

void TreeNode::add(std::string &&name)
{
    children->append(Glib::make_refptr_for_instance(new Subsystem(std::move(name), this)));
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
