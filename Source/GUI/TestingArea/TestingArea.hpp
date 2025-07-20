/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Main Window's Testing UI area
 * @author Oliver Dixon
 * @date 2025-07-09
 * @version Development
 */

#ifndef TESTINGAREA_HPP
#define TESTINGAREA_HPP

#include <glibmm/refptr.h>
#include <gtkmm/singleselection.h>

#include "../../DereferencingEqualityFunctor.hpp"
#include "../../Storage/Subsystem.hpp"
#include "../../UserTesting/Google/GoogleTestListener.hpp"
#include "../IWindowArea.hpp"

namespace optifol
{

class TestingArea :
        public IWindowArea
{
public:
    explicit TestingArea(Gtk::Builder& builder);

    void select_model(const Glib::RefPtr<const Subsystem> &new_subsystem) override;

    void deselect_model() override;

    const Subsystem *observe_active_subsystem() const noexcept override;

    guint get_selected_index() const override;

    void accept_new_result(std::unique_ptr<TestResult>&& test_result);

    void propagate_pending_results();

private:
    static const log4cxx::LoggerPtr area_logger;

    Glib::RefPtr<const Subsystem> active_subsystem;
    Glib::RefPtr<Gio::ListStore<Requirement>> data_model;
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();

    std::unique_ptr<TestListenerBase> test_listener;

    std::unordered_map<TestResult *, std::shared_ptr<TestResult>, std::hash<TestResult>,
        DereferencingEqualityFunctor<const TestResult *, const TestResult>> received_test_results;
};

} // namespace optifol

#endif // TESTINGAREA_HPP
