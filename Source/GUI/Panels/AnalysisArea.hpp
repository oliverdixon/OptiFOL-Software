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
#include <gtkmm/singleselection.h>
#include <gtkmm/treelistmodel.h>

#include "../ContextButtonCorrespondence.hpp"
#include "../../Storage/AnalysisGroup.hpp"

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

    void select_model(const Glib::RefPtr<Gio::ListStore<AnalysisGroup>>& new_model);

    void deselect_model();

private:
    Glib::RefPtr<Gio::ListStore<AnalysisGroup>> data_model;
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();
    Glib::RefPtr<Gtk::TreeListModel> tree_model;

    static const char * const area_name;

    std::pair<Gtk::Widget*, Gtk::Widget*> on_off_widgets;

    Gtk::ColumnView * groups_view;

    ContextButtonCorrespondence context_menu;

    void configure_new_analysis_group_popover(Gtk::Builder& builder) const;

    void configure_delete_analysis_group_popover(Gtk::Builder& builder) const;

    static Glib::RefPtr<Gio::ListModel> analysis_group_expand(const Glib::RefPtr<Glib::ObjectBase> &item);
};

}

#endif
