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
    on_off_widgets(GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "testing_advice_unselected"),
            GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "testing_content")),
    test_groups_view(GTKHelpers::get_widget<Gtk::ColumnView>(area_name, builder, "test_groups_view")),
    run_tests_button(GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "run_tests")),
    run_tests_output_buffer(
            GTKHelpers::get_widget<Gtk::TextView>(area_name, builder, "run_tests_output")->get_buffer()),
    test_listener(std::make_unique<GoogleTestListener>(sigc::mem_fun(*this, &TestingArea::accept_new_result),
            sigc::mem_fun(*this, &TestingArea::propagate_pending_results)))
{
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);

    run_tests_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingArea::execute_tests));

    test_groups_view->set_model(selection_model);

    const auto columns = test_groups_view->get_columns();
    const auto column_count = columns->get_n_items();

    for (guint position = 0; position < column_count; ++position) {
        Glib::RefPtr<Gtk::ColumnViewColumn> column = nullptr;

        if ((column = columns->get_typed_object<Gtk::ColumnViewColumn>(position)) != nullptr) {
            const auto &gtk_id = column->get_id();
            const auto factory = Gtk::SignalListItemFactory::create();

            if (gtk_id == "test_requirement_name") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_expandable_label, false));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                        { GTKHelpers::on_bind_expandable_name(list_item, tree_model); });

            } else if (gtk_id == "test_target_executable") {

                using ReturnType = Glib::ustring;

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, true));
                factory->signal_bind().connect(sigc::bind(&TestingArea::on_bind_test_property<ReturnType>,
                    sigc::mem_fun(static_cast<TestPropertyGetter<ReturnType>>(&Test::property_target_executable))));

            } else if (gtk_id == "test_suite") {

                using ReturnType = std::optional<Glib::ustring>;

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, true));
                factory->signal_bind().connect(sigc::bind(&TestingArea::on_bind_test_property_opt<ReturnType>,
                    sigc::mem_fun(static_cast<TestPropertyGetter<ReturnType>>(&Test::property_test_suite))));

            } else if (gtk_id == "test_name") {

                using ReturnType = Glib::ustring;

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, true));
                factory->signal_bind().connect(sigc::bind(&TestingArea::on_bind_test_property<ReturnType>,
                    sigc::mem_fun(static_cast<TestPropertyGetter<ReturnType>>(&Test::property_name))));

            } else
                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
        }
    }
}

void TestingArea::select_model(const Glib::RefPtr<const Subsystem> &new_subsystem)
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);

    active_subsystem = new_subsystem;
    data_model = active_subsystem->test_groups;
    tree_model = Gtk::TreeListModel::create(data_model, sigc::ptr_fun(&TestingArea::test_group_expand), true, true);
    selection_model->set_model(tree_model);
}

void TestingArea::deselect_model()
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);

    active_subsystem = nullptr;
    tree_model = nullptr;
    data_model = nullptr;
    selection_model->set_model(nullptr);
}

const Subsystem *TestingArea::observe_active_subsystem() const noexcept
{
    return active_subsystem.get();
}

guint TestingArea::get_selected_index() const
{
    return selection_model->get_selected();
}

void TestingArea::accept_new_result(std::unique_ptr<TestResult> &&test_result)
{
    received_test_results.emplace(test_result.get(), std::move(test_result));
}

void TestingArea::propagate_pending_results()
{
    const auto group_count = data_model->get_n_items();

    for (guint group_idx = 0; group_idx < group_count; ++group_idx) {
        const auto grouped_requirements = data_model->get_item(group_idx)->requirements;
        const auto requirement_count = grouped_requirements->get_n_items();

        for (guint requirement_idx = 0; requirement_idx < requirement_count; ++requirement_idx) {
            const auto requirement = grouped_requirements->get_item(requirement_idx);
            const auto &test = requirement->observe_test();

            if (test.has_value() == true) {
                const auto &glib_suite_name = test->property_test_suite().get_value();
                const auto &glib_test_name = test->property_name().get_value();

                const auto it = received_test_results.find(
                        std::make_pair(std::string_view(glib_suite_name->c_str(), glib_suite_name->bytes()),
                                std::string_view(glib_test_name.c_str(), glib_test_name.bytes())));

                if (it != received_test_results.cend()) {
                    requirement->emplace_test_result(it->second);
                    area_logger->debug("Matched parsed test result \"" + *glib_suite_name + '.' + glib_test_name +
                            "\" with requirement \"" + requirement->property_name().get_value() + "\".");
                } else
                    area_logger->debug("Could not match requirement \"" + requirement->property_name().get_value() +
                            "\" with any parsed test.");
            }
        }
    }
}

void TestingArea::execute_tests()
{
    test_executor.emplace("",
            std::vector<std::string>{"cmake-build-debug/OptifolTesting",
                    "--gtest_filter=FOLParserTest.*", // TODO get from selected requirement/test group.
                    "--gtest_stream_result_to=127.0.0.1:12345"},
            std::vector<std::string>{}, run_tests_output_buffer, [this](const int) { test_executor.reset(); });
}

Glib::RefPtr<Gio::ListModel> TestingArea::test_group_expand(const Glib::RefPtr<Glib::ObjectBase> &item)
{
    const auto candidate = std::dynamic_pointer_cast<TestGroup>(item);

    if (candidate != nullptr)
        return candidate->requirements;

    return nullptr;
}

} // namespace optifol
