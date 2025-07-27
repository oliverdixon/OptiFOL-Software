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
#include "../../Storage/TestGroup.hpp"
#include "../GTKHelpers.hpp"
#include "../ProcessExecutor.hpp"

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
    run_tests_output_buffer(
        GTKHelpers::get_widget<Gtk::TextView>(area_name, builder, "run_tests_output")->get_buffer()
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
    tree_model = Gtk::TreeListModel::create(active_subsystem->get_test_groups(),
        sigc::ptr_fun(&TestGroup::get_expanded_list<TestGroup>), true, true);
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

void TestingArea::accept_new_result(std::unique_ptr<TestResult> &&test_result)
{
    received_test_results.emplace(test_result.get(), std::move(test_result));
}

void TestingArea::propagate_pending_results()
{
    const auto data_model = active_subsystem->get_test_groups();
    const auto group_count = data_model->get_n_items();

    for (guint group_idx = 0; group_idx < group_count; ++group_idx) {
        const auto grouped_requirements = data_model->get_item(group_idx);
        grouped_requirements->for_each(
            [this](Requirement &requirement)
            {
                const auto &test = requirement.observe_test();

                if (test.has_value() == true)
                    return;

                const auto &glib_suite_name = test->property_test_suite().get_value();
                const auto &glib_test_name = test->property_name().get_value();

                const auto it = received_test_results.find(
                        std::make_pair(std::string_view(glib_suite_name->c_str(), glib_suite_name->bytes()),
                                std::string_view(glib_test_name.c_str(), glib_test_name.bytes())));

                if (it != received_test_results.cend()) {
                    requirement.emplace_test_result(it->second);
                    area_logger->debug("Matched parsed Test result \"" + *glib_suite_name + '.' + glib_test_name +
                            "\" with Requirement \"" + requirement.property_name().get_value() + "\".");
                } else
                    area_logger->debug("Could not match Requirement \"" + requirement.property_name().get_value() +
                            "\" with any parsed Test.");
            }
        );
    }
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

std::optional<std::pair<const std::optional<Test> &, Gtk::Label *>> TestingArea::bind_helper(
        const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto requirement = std::dynamic_pointer_cast<Requirement>(list_item->get_item());

    if (requirement == nullptr)
        return std::nullopt;

    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());

    if (label == nullptr)
        return std::nullopt;

    return std::make_pair(std::cref(requirement->observe_test()), label);
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
                factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem> & list_item)
                {
                    const auto [requirement, label] = Requirement::requirement_bind_helper(*list_item);
                    GTKHelpers::bind_any_property(
                        sigc::mem_fun(static_cast<TestGetter<Glib::ustring>>(&Test::property_target_executable)),
                        *requirement->observe_test(), label);
                });

            } else if (gtk_id == "test_suite") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem> & list_item)
                {
                    const auto [requirement, label] = Requirement::requirement_bind_helper(*list_item);
                    GTKHelpers::bind_any_property(
                        sigc::mem_fun(static_cast<TestGetter<std::optional<Glib::ustring>>>(&Test::property_test_suite)),
                        *requirement->observe_test(), label);
                });

            } else if (gtk_id == "test_name") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem> & list_item)
                {
                    const auto [requirement, label] = Requirement::requirement_bind_helper(*list_item);
                    GTKHelpers::bind_any_property(
                        sigc::mem_fun(static_cast<TestGetter<Glib::ustring>>(&Test::property_name)),
                        *requirement->observe_test(), label);
                });

            } else if (gtk_id == "test_status") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem> & list_item) -> void
                {
                    const auto [requirement, label] = Requirement::requirement_bind_helper(*list_item);
                    const auto& test = requirement->observe_test();
                    if (label == nullptr || test.has_value() == false)
                        return;

                    Glib::Binding::bind_property(
                        test->property_result(),
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

#if 0
    selection_model->signal_selection_changed().connect(
        [this](const guint position, const guint n_items)
        {
            if (n_items == 0)
                context_menu.disable_action("new_test_group");
            else
                context_menu.enable_action("new_test_group");
        }
    );

    selection_model->signal_items_changed().connect(
        [this](guint, const guint removed, guint)
        {
            if (removed > 0) {
                // If anything was removed from the model, just disable everything out of an abundance of caution.
                context_menu.disable_action("new_test_group");
                context_menu.disable_action("run_tests");
                selection_model->unselect_all();
            }
        }
    );
#endif
}

} // namespace optifol
