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

TestingArea::TestingArea(Gtk::Builder &builder)
{
}

void TestingArea::select_model(const Glib::RefPtr<const Subsystem> &subsystem)
{
}

void TestingArea::deselect_model()
{
}

const Subsystem *TestingArea::observe_active_subsystem() const noexcept
{
    return nullptr; // TODO
}

} // namespace optifol
