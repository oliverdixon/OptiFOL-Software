/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the ITestModelNode interface
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#ifndef TESTMODELNODE_HPP
#define TESTMODELNODE_HPP

#include <gtkmm/treelistmodel.h>

namespace optifol
{

/**
 * @class ITestModelNode
 * @brief Provides an interface for participation in the hierarchy of runnable software-level tests. Such nodes expose a
 *  retrieval function to get child Gtk::TreeListModel objects. These can be accessed through a Glib-friendly static
 *  context with @ref ITestModelNode::get_given_tree.
 */
class ITestModelNode
{
public:
    /**
     * @brief Destruct the ITestModelNode.
     */
    virtual ~ITestModelNode() = default;

    /**
     * @brief Retrieve the Gtk::TreeListModel containing the children of the derived type from a static context.
     * @param node The ITestModelNode, wrapped in a Glib::ObjectBase.
     * @return The tree, or an empty Glib::RefPtr for a leaf node or node of incorrect type.
     */
    static Glib::RefPtr<Gtk::TreeListModel> get_given_tree(const Glib::RefPtr<Glib::ObjectBase>& node) noexcept
    {
        const auto typed_node = dynamic_cast<ITestModelNode *>(node.get());
        if (typed_node == nullptr)
            return nullptr;

        return typed_node->get_tree();
    }

private:
    /**
     * @brief Retrieve the Gtk::TreeListModel containing the children of the derived type from a non-static context.
     * @return The tree, or an empty Glib::RefPtr for a leaf node.
     */
    [[nodiscard]] virtual Glib::RefPtr<Gtk::TreeListModel> get_tree() const noexcept = 0;
};

} // namespace optifol

#endif // TESTMODELNODE_HPP
