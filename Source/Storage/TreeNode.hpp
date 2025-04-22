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
    explicit TreeNode(Gio::ListStore<Subsystem> * parent);

    [[nodiscard]] Glib::RefPtr<Gio::ListStore<Subsystem>> get_children();

    [[nodiscard]] Glib::RefPtr<const Gio::ListStore<Subsystem>> get_children() const;

    void add(std::string&& name, const std::chrono::system_clock::time_point& created_time =
        std::chrono::system_clock::now(), const std::chrono::system_clock::time_point& last_modified_time =
        std::chrono::system_clock::now()) const;

    [[nodiscard]] Gio::ListStore<Subsystem> * get_parent() const;

private:
    Glib::RefPtr<Gio::ListStore<Subsystem>> children;
    Gio::ListStore<Subsystem> * parent;
};

}

#endif
