/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Analysis and Optimisation view in the Optifol Main Window
 * @author Oliver Dixon
 * @date 2025-04-27
 * @version Development
 */

#ifndef ANALYSISAREA_HPP
#define ANALYSISAREA_HPP

#include <gtkmm/builder.h>
#include <gtkmm/columnview.h>

#include "../ContextButtonCorrespondence.hpp"

namespace optifol
{

class AnalysisArea :
        public sigc::trackable
{
public:
    /**
      * @brief Construct a new compartmentalised area for displaying and managing sets of subsystem requirements
      * @param builder The GTK builder attached to the main window
      */
    explicit AnalysisArea(Gtk::Builder& builder);

private:
    static const char * const area_name;

    Gtk::ColumnView * groups_view;

    ContextButtonCorrespondence context_menu;
};

}

#endif
