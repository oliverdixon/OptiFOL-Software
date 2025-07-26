/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/26/25.
//

#ifndef TESTINGRUNTESTSPOPOVER_HPP
#define TESTINGRUNTESTSPOPOVER_HPP

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/notebook.h>
#include <gtkmm/popover.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <log4cxx/logger.h>

#include "../ProcessExecutor.hpp"

namespace optifol
{

class TestingArea;

class TestingRunTestsPopover
{
public:
    TestingRunTestsPopover(Gtk::Builder& builder, TestingArea& testing_area);

private:
    struct DiscoveryPage
    {
        explicit DiscoveryPage(const std::string& tab_name);

        Gtk::Label tab_label;
        Gtk::ScrolledWindow scrolled_window;
        Gtk::TextView text_view;
    };

    /**
     * @brief Handle a click of the <i>Confirm</i> by attempting to create a Requirement with the given characteristics.
     */
    void confirm_button_clicked() const;

    /**
     * @brief Handle a click of the <i>Cancel</i> button by discarding all input and closing the popover.
     */
    void cancel_button_clicked() const;

    /**
     * @brief Clear all user fields in the popover
     */
    void clear_inputs() const;

    void show_popover();

    void discover_tests_clicked();

    void build_new_discovery_page(const DiscoveryPage& discovery);

    void discover_executable(const std::string &executable_name, const Glib::RefPtr<Gtk::TextBuffer> &output_buffer);

    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    TestingArea& testing_area;

    Gtk::Popover * const my_popover;
    Gtk::Button * const confirm_button;
    Gtk::Button * const cancel_button;
    Gtk::Entry * const test_group_name_entry;
    Gtk::Notebook * const discovery_notebook;
    Gtk::Button * const discover_button;
    Gtk::Box * const discovery_box;

    std::vector<DiscoveryPage> discovery_pages;
    std::unordered_map<std::string_view, std::unique_ptr<ProcessExecutor>> discovery_executor_pool;
};

} // namespace optifol

#endif // TESTINGRUNTESTSPOPOVER_HPP
