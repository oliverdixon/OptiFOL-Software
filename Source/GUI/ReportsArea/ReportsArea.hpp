/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Main Window's Reporting and Compliance area
 * @author Oliver Dixon
 * @date 2025-07-08
 * @version Development
 */

#ifndef REPORTSAREA_HPP
#define REPORTSAREA_HPP

#include <gtkmm/builder.h>
#include <gtkmm/columnview.h>

#include "../../Storage/Subsystem.hpp"
#include "../ContextButtonCorrespondence.hpp"
#include "../IWindowArea.hpp"
#include "ReportsAreaGenerateLaTeXPopover.hpp"

namespace optifol
{

class ReportsArea :
        public IWindowArea
{
public:
    explicit ReportsArea(Gtk::Builder& builder);

    void select_model(const Glib::RefPtr<const Subsystem> &subsystem) override;

    void deselect_model() override;

    const Subsystem * observe_active_subsystem() const noexcept;

private:
    static const char * const area_name;

    std::pair<Gtk::Widget*, Gtk::Widget*> on_off_widgets;

    Gtk::ColumnView * view;

    ContextButtonCorrespondence context_menu;

    Glib::RefPtr<const Subsystem> active_subsystem;

    ReportsAreaGenerateLaTeXPopover generate_latex_popover;
};

} // namespace optifol

#endif // REPORTSAREA_HPP
