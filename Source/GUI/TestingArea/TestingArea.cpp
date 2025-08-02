/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Main Window's Testing UI area
 * @author Oliver Dixon
 * @date 2025-07-09
 * @version Development
 */

#include "TestingArea.hpp"
#include "../../UserTesting/Modelling/TestGroup.hpp"
#include "../GTKHelpers.hpp"

namespace optifol
{

const log4cxx::LoggerPtr TestingArea::area_logger = Logging::get_logger({"GUI", "TestingCompliance"});
const char *const TestingArea::area_name = "Testing and Compliance Area";

TestingArea::TestingArea(Gtk::Builder &builder) :
    test_groups_view(GTKHelpers::get_widget<Gtk::ColumnView>(area_name, builder, "test_groups_view")),
    context_menu(
        test_groups_view,
        GTKHelpers::get_object<Gio::Menu>(area_name, builder, "test_groups_context_menu"),
        {
            {
                "run_tests",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "run_tests"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "run_tests_popover"),
                true
            }
        }
    ),
    on_off_widgets(
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "testing_advice_unselected"),
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "testing_content")
    ),
    run_tests_popover(builder, *this)
{
    configure_selection_model();
    test_groups_view->set_model(selection_model);

    configure_columns();
}

void TestingArea::select_model(const Glib::RefPtr<Subsystem> &new_subsystem)
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);

    active_subsystem = new_subsystem;
    tree_model = Gtk::TreeListModel::create(active_subsystem->get_test_groups(), &ITestModelNode::get_given_tree, true);
    selection_model->set_model(tree_model);
}

void TestingArea::deselect_model()
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);

    active_subsystem = nullptr;
    tree_model = nullptr;
    selection_model->set_model(nullptr);
}

Subsystem *TestingArea::observe_active_subsystem() noexcept
{
    return active_subsystem.get();
}

const Subsystem *TestingArea::observe_active_subsystem() const noexcept
{
    return active_subsystem.get();
}

Glib::RefPtr<TestGroup> TestingArea::get_selection() const
{
    const auto selected_item = selection_model->get_selected_item();

    if (selected_item == nullptr)
        throw std::runtime_error("No item selected in the selection model.");

    const auto selected_group = std::dynamic_pointer_cast<TestGroup>(selected_item);

    if (selected_group == nullptr)
        throw std::runtime_error("Selected item is not a Test Group.");

    return selected_group;
}

void TestingArea::configure_columns() const
{
    const auto columns = test_groups_view->get_columns();
    const auto column_count = columns->get_n_items();

    for (guint position = 0; position < column_count; ++position) {
        Glib::RefPtr<Gtk::ColumnViewColumn> column = nullptr;

        if ((column = columns->get_typed_object<Gtk::ColumnViewColumn>(position)) != nullptr) {
            const auto &gtk_id = column->get_id();
            const auto factory = Gtk::SignalListItemFactory::create();

            if (gtk_id == "test_requirement_name") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_expandable_label, false));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                        { StorageObjectBase::bind_name_property_expandable(list_item, tree_model); });

            } else if (gtk_id == "test_target_executable") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem> & list_item) -> void
                {
                    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
                    const auto typed_test = std::dynamic_pointer_cast<Test>(list_item->get_item());

                    if (label == nullptr || typed_test == nullptr)
                        return;

                    Glib::Binding::bind_property(
                        typed_test->property_target_executable_name(),
                        label->property_label(),
                        Glib::Binding::Flags::SYNC_CREATE
                    );
                });

            } else if (gtk_id == "test_fixture") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem> & list_item) -> void
                {
                    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
                    const auto typed_test = std::dynamic_pointer_cast<Test>(list_item->get_item());

                    if (label == nullptr || typed_test == nullptr)
                        return;

                    Glib::Binding::bind_property(
                        typed_test->property_fixture(),
                        label->property_label(),
                        Glib::Binding::Flags::SYNC_CREATE
                    );
                });

            } else if (gtk_id == "test_status") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem> & list_item) -> void
                {
                    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
                    const auto typed_test = std::dynamic_pointer_cast<Test>(list_item->get_item());

                    if (label == nullptr || typed_test == nullptr)
                        return;

                    Glib::Binding::bind_property(
                        typed_test->property_result(),
                        label->property_label(),
                        Glib::Binding::Flags::SYNC_CREATE,
                        [](const std::shared_ptr<TestResult> &result) -> std::optional<Glib::ustring>
                        {
                            if (result == nullptr)
                                return "Unknown";

                            return result->has_passed() ? "Passed" : "Failed";
                        }
                    );
                });

            } else
                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
        }
    }
}

void TestingArea::configure_selection_model() const
{
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
}

} // namespace optifol
