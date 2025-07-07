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

#include <glibmm/iochannel.h>
#include <gtkmm/builder.h>
#include <gtkmm/columnview.h>

#include "../../Storage/Subsystem.hpp"
#include "../ContextButtonCorrespondence.hpp"
#include "../DocumentGeneration/LaTeXGeneratorManager.hpp"

namespace optifol
{

class ReportsArea :
        public sigc::trackable
{
public:
    explicit ReportsArea(Gtk::Builder& builder);

    void select_model(const Glib::RefPtr<const Subsystem>& subsystem);

    void deselect_model();

private:
    void configure_generate_latex_popover(Gtk::Builder &builder);

    static const char * const area_name;

    std::pair<Gtk::Widget*, Gtk::Widget*> on_off_widgets;

    Gtk::ColumnView * view;

    ContextButtonCorrespondence context_menu;

    std::unique_ptr<LaTeXGeneratorManager> latexmk_stdout_manager;
    std::unique_ptr<LaTeXGeneratorManager> latexmk_stderr_manager;

    struct LatexmkCommand
    {
        static const std::string base_command;
        static const std::string template_path;
        std::string output_path;
    };

    LatexmkCommand latexmk_command;
};

} // namespace optifol

#endif // REPORTSAREA_HPP
