/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the tree node abstract storage object
 * @author Oliver Dixon
 * @date 2025-04-26
 * @version Development
 */

#ifndef TREENODE_HPP
#define TREENODE_HPP

#include <chrono>
#include <giomm/liststore.h>

namespace optifol
{

class Subsystem;

/**
 * @class TreeNode
 * @brief The TreeNode is an abstract storage object to aid in the nesting of subsystems to arbitrary depths.
 */
class TreeNode
{
public:
    /**
     * @brief Destruct the TreeNode
     */
    virtual ~TreeNode() = default;

    [[nodiscard]] Glib::RefPtr<Gio::ListStore<Subsystem>> get_children();

    [[nodiscard]] Glib::RefPtr<const Gio::ListStore<Subsystem>> get_children() const;

    void add(std::string &&name);

    [[nodiscard]] const TreeNode *get_parent() const;

    [[nodiscard]] TreeNode *get_parent();

    [[nodiscard]] virtual std::string get_path() const = 0;

protected:
    /**
     * @brief Create a new TreeNode without a parent
     */
    TreeNode();

    /**
     * @brief Create a new TreeNode with a parent
     * @param parent A weak pointer to the parent
     */
    explicit TreeNode(TreeNode *parent);

private:
    Glib::RefPtr<Gio::ListStore<Subsystem>> children;

    TreeNode *const parent = nullptr;
};

} // namespace optifol

#endif
