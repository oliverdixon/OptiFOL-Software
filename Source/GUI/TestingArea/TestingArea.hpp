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

    void select_model(const Glib::RefPtr<const Subsystem> &subsystem) override;

    void deselect_model() override;

    const Subsystem *observe_active_subsystem() const noexcept override;

    guint get_selected_index() const override;

private:
    std::unique_ptr<ITestListener> test_listener = std::make_unique<GoogleTestListener>();
};

} // namespace optifol

#endif // TESTINGAREA_HPP
