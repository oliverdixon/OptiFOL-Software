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

namespace optifol
{

const log4cxx::LoggerPtr TestingArea::area_logger = Logging::get_logger({"GUI", "Testing"});

TestingArea::TestingArea(Gtk::Builder &builder) :
    test_listener(std::make_unique<GoogleTestListener>(sigc::mem_fun(*this, &TestingArea::accept_new_result),
            sigc::mem_fun(*this, &TestingArea::propagate_pending_results)))
{
}

void TestingArea::select_model(const Glib::RefPtr<const Subsystem> &new_subsystem)
{
    active_subsystem = new_subsystem;
    data_model = active_subsystem->requirements;
    selection_model->set_model(data_model);
}

void TestingArea::deselect_model()
{
    active_subsystem = nullptr;
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
    // TODO this should go over test groups, not requirements.

    const auto requirement_count = data_model->get_n_items();
    for (guint requirement_idx = 0; requirement_idx < requirement_count; ++requirement_idx) {
        const auto requirement = data_model->get_item(requirement_idx);
        const auto &test = requirement->observe_test();

        if (test.has_value() == true) {
            const auto& glib_suite_name = test->property_test_suite().get_value();
            const auto& glib_test_name = test->property_test_name().get_value();

            const auto it = received_test_results.find(std::make_pair(
                std::string_view(glib_suite_name->c_str(), glib_suite_name->bytes()),
                std::string_view(glib_test_name.c_str(), glib_test_name.bytes())
            ));

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

} // namespace optifol
