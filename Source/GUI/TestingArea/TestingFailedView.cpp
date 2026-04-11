/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Testing and Compliance Failed Test Results view
 * @author Oliver Dixon
 * @date 2025-08-10
 * @version Development
 */

#include "TestingFailedView.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "../Storage/Subsystem.hpp"

namespace optifol
{

const log4cxx::LoggerPtr TestingFailedView::area_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "FailedTests"});
const char *const TestingFailedView::area_name = "Testing and Compliance Failed Tests Area";

TestingFailedView::TestingFailedView(Gtk::Builder &builder) :
    table(GTKHelpers::get_widget<Gtk::ColumnView>(area_name, builder, "test_groups_failed_view")),
    container(GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "test_groups_failed_container"))
{
    table->set_model(selection_model);
    configure_columns();
}

Subsystem *TestingFailedView::get_active_subsystem() noexcept
{
    return active_subsystem.get();
}

const Subsystem *TestingFailedView::observe_active_subsystem() const noexcept
{
    return active_subsystem.get();
}

void TestingFailedView::select_model(const Glib::RefPtr<Subsystem> &new_subsystem)
{
    active_subsystem = new_subsystem;
    tree_model = Gtk::TreeListModel::create(
            active_subsystem->get_test_groups(), &ITestModelNode::get_given_results_tree, true, true);
    selection_model->set_model(tree_model);
    tree_model->signal_items_changed().connect(sigc::mem_fun(*this, &TestingFailedView::handle_model_change));
}

void TestingFailedView::deselect_model()
{
    active_subsystem = nullptr;
    tree_model = nullptr;
    selection_model->set_model(nullptr);
}

void TestingFailedView::configure_columns() const
{
    const auto columns = table->get_columns();
    const auto column_count = columns->get_n_items();

    for (guint position = 0; position < column_count; ++position) {
        Glib::RefPtr<Gtk::ColumnViewColumn> column = nullptr;

        if ((column = columns->get_typed_object<Gtk::ColumnViewColumn>(position)) != nullptr) {
            const auto &gtk_id = column->get_id();
            const auto factory = Gtk::SignalListItemFactory::create();

            if (gtk_id == "failed_test_message") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_expandable_label, false));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item) noexcept
                        { StorageObjectBase::bind_name_property_expandable(list_item, tree_model); });

            } else if (gtk_id == "failed_test_source_file") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect(
                        [](const Glib::RefPtr<Gtk::ListItem> &list_item) noexcept -> void
                        {
                            const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
                            const auto typed_result =
                                    dynamic_cast<const PartialTestResult *>(list_item->get_item().get());

                            if (label == nullptr || typed_result == nullptr)
                                return;

                            Glib::Binding::bind_property(typed_result->property_file(),
                                    label->property_label(), Glib::Binding::Flags::SYNC_CREATE);
                        });

            } else if (gtk_id == "failed_test_source_line") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect(
                        [](const Glib::RefPtr<Gtk::ListItem> &list_item) noexcept -> void
                        {
                            const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
                            const auto typed_result =
                                    dynamic_cast<const PartialTestResult *>(list_item->get_item().get());

                            if (label == nullptr || typed_result == nullptr)
                                return;

                            Glib::Binding::bind_property(typed_result->property_line(),
                                    label->property_label(), Glib::Binding::Flags::SYNC_CREATE,
                                    [](const guint line) -> std::optional<Glib::ustring>
                                    { return std::to_string(line); });
                        });

            } else
                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
        }
    }
}

void TestingFailedView::handle_model_change(
        const guint initial_index, const guint removed_count, const guint added_count) const noexcept
{
    std::ignore = initial_index;
    std::ignore = removed_count;
    std::ignore = added_count;

    /*
     * If the tree model doesn't exist, we always want to disable the failed view: no model means no failed
     * tests.
     *
     * Recall that the number of items in the failed model is lower-bounded by the number of test groups. Once
     * the number of items in the failed model reaches the number of test groups, we know that all test groups
     * must be empty. Since they are empty, there are no failure records, thus the container should be hidden.
     */
    const bool should_display = tree_model != nullptr &&
            tree_model->get_n_items() > active_subsystem->get_test_groups()->get_n_items();

    if (container->get_visible() != should_display)
        container->set_visible(should_display);
}

} // namespace optifol
