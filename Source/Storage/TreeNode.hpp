/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef TREENODE_HPP
#define TREENODE_HPP

#include <chrono>
#include <giomm/liststore.h>

namespace optifol
{

class Subsystem;

class TreeNode
{
public:
    virtual ~TreeNode() = default;

    explicit TreeNode(TreeNode * parent);

    [[nodiscard]] Glib::RefPtr<Gio::ListStore<Subsystem>> get_children();

    [[nodiscard]] Glib::RefPtr<const Gio::ListStore<Subsystem>> get_children() const;

    void add(std::string&& name, const std::chrono::system_clock::time_point& created_time =
        std::chrono::system_clock::now(), const std::chrono::system_clock::time_point& last_modified_time =
        std::chrono::system_clock::now());

    [[nodiscard]] const TreeNode * get_parent() const;

    [[nodiscard]] TreeNode * get_parent();

    [[nodiscard]] virtual std::string get_path() const = 0;

private:
    Glib::RefPtr<Gio::ListStore<Subsystem>> children;
    TreeNode * const parent;
};

}

#endif
