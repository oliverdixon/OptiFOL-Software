/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/29/25.
//

#ifndef MANAGETESTSPOPOVER_HPP
#define MANAGETESTSPOPOVER_HPP

#include <giomm/liststore.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/columnview.h>
#include <gtkmm/listitem.h>
#include <gtkmm/popover.h>
#include <gtkmm/singleselection.h>
#include <log4cxx/logger.h>

namespace optifol
{

class Test;
class RequirementsIndexArea;

class ManageTestsPopover
{
public:
    ManageTestsPopover(RequirementsIndexArea& parent_area, Gtk::Builder& builder);

private:
    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    /**
     * @brief Handle a click of the <i>Confirm</i> by hiding the popover.
     */
    void confirm_button_clicked() const;

    void new_test_clicked() const;

    void delete_test_clicked() const;

    static void bind_test_executable(const Glib::RefPtr<Gtk::ListItem>& list_item);

    static void bind_text_fixture(const Glib::RefPtr<Gtk::ListItem>& list_item);

    RequirementsIndexArea& parent_area;

    Gtk::Popover * const popover;
    Gtk::Button * const confirm_button;
    Gtk::Button * const new_test_button;
    Gtk::Button * const duplicate_test_button;
    Gtk::Button * const delete_test_button;
    Gtk::ColumnView * const view;

    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();
    Glib::RefPtr<Gio::ListStore<Test>> test_spec_model;
};

} // namespace optifol

#endif // MANAGETESTSPOPOVER_HPP
