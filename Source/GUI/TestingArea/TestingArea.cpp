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
    return nullptr; // TODO
}

guint TestingArea::get_selected_index() const
{
    // TODO
    return GTK_INVALID_LIST_POSITION;
}

void TestingArea::accept_new_result(std::unique_ptr<TestResult> &&test_result)
{
    pending_test_results.emplace(test_result.get(), std::move(test_result));
}

void TestingArea::propagate_pending_results()
{
    // TODO this should go over test groups, not requirements.

    const auto requirement_count = data_model->get_n_items();
    for (guint requirement_idx = 0; requirement_idx < requirement_count; ++requirement_idx) {
        const auto requirement = data_model->get_item(requirement_idx);
        const auto &test = requirement->observe_test();

        const auto& glib_suite_name = test->property_test_suite().get_value();
        const auto& glib_test_name = test->property_test_name().get_value();

        const std::string_view suite_name{glib_suite_name->c_str(), glib_suite_name->bytes()};
        const std::string_view test_name{glib_test_name.c_str(), glib_test_name.bytes()};

        if (test.has_value() == true) {
            const auto it = pending_test_results.find(
                    std::make_pair(suite_name, test_name));

            if (it != pending_test_results.cend()) {
                requirement->emplace_test_result(std::move(it->second));

                /*
                 * TODO: MAJOR BUG: multiple requirements may use the same test. So we should be used shared_ptr,
                 *  not unique_ptr, in the GoogleTest infrastructure. There can still be a notion of "stealing", just
                 *  using shared_ptr(shared_ptr&&) move c'tor.
                 */
                pending_test_results.erase(it);
            }
        }
    }
}

} // namespace optifol
