/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 8/2/25.
//

#ifndef TESTMODELNODE_HPP
#define TESTMODELNODE_HPP

#include <gtkmm/treelistmodel.h>

namespace optifol
{

class ITestModelNode
{
public:
    virtual ~ITestModelNode() = default;

    static Glib::RefPtr<Gtk::TreeListModel> get_given_tree(const Glib::RefPtr<Glib::ObjectBase>& node) noexcept
    {
        const auto typed_node = dynamic_cast<ITestModelNode *>(node.get());
        if (typed_node == nullptr)
            return nullptr;

        return typed_node->get_tree();
    }

private:
    [[nodiscard]] virtual Glib::RefPtr<Gtk::TreeListModel> get_tree() const noexcept = 0;
};

} // namespace optifol

#endif // TESTMODELNODE_HPP
