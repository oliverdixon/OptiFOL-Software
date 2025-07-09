/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Optifol GTK main window
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/builder.h>

#include "AnalysisArea/AnalysisArea.hpp"
#include "ProjectHierarchyPane/ProjectHierarchyPane.hpp"
#include "ReportsArea/ReportsArea.hpp"
#include "RequirementsIndexArea/RequirementsIndexArea.hpp"
#include "TestingArea/TestingArea.hpp"

namespace optifol
{

class MainWindow :
        public Gtk::ApplicationWindow
{
public:
    MainWindow();

private:
    Glib::RefPtr<Gtk::Builder> builder;
    static const char * const area_name;

    Gtk::Box * const root_grid;

    std::unique_ptr<ProjectHierarchyPane> project_hierarchy_pane;

    std::unique_ptr<RequirementsIndexArea> requirements_index_area;
    std::unique_ptr<AnalysisArea> analysis_area;
    std::unique_ptr<TestingArea> testing_area;
    std::unique_ptr<ReportsArea> reports_area;
};

}

#endif
