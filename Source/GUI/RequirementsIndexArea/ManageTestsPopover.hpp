/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the ManageTestsPopover GUI popover
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#ifndef MANAGETESTSPOPOVER_HPP
#define MANAGETESTSPOPOVER_HPP

#include <giomm/liststore.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/columnview.h>
#include <gtkmm/entry.h>
#include <gtkmm/listitem.h>
#include <gtkmm/popover.h>
#include <gtkmm/singleselection.h>
#include <log4cxx/logger.h>
#include <unordered_set>

#include "../../DereferencingEqualityFunctor.hpp"
#include "../../UserTesting/Discovery/DiscoveryTestExecutable.hpp"

namespace optifol
{

class Test;
class RequirementsIndexArea;
class TestSpecificationEntry;

class ManageTestsPopover
{
public:
    explicit ManageTestsPopover(Gtk::Builder& builder);

    void set_model(const Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>>& model);

    /**
     * @brief Provide a human-readable summary of a model containing tests or specifications thereof.
     * @param test_model The test model to query.
     * @see Test
     * @see TestSpecificationEntry
     */
    static std::string format_test_summary(const Gio::ListModel &test_model) noexcept;

private:
    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    /**
     * @brief Handle a click of the <i>Confirm</i> by hiding the popover.
     */
    void confirm_button_clicked() const;

    void new_test_clicked() const;

    void delete_test_clicked() const;

    void duplicate_test_clicked() const;

    static void setup_fixtures_combo(const Glib::RefPtr<Gtk::ListItem> &list_item);

    void bind_test_executable(const Glib::RefPtr<Gtk::ListItem> &list_item);

    static void bind_test_fixture(const Glib::RefPtr<Gtk::ListItem>& list_item);

    static void bind_test_name(const Glib::RefPtr<Gtk::ListItem>& list_item);

    void handle_executable_change(const Glib::RefPtr<TestSpecificationEntry> &test_spec, const Gtk::Entry *exe_entry);

    Gtk::Popover * const popover;
    Gtk::Button * const confirm_button;
    Gtk::Button * const new_test_button;
    Gtk::Button * const duplicate_test_button;
    Gtk::Button * const delete_test_button;
    Gtk::ColumnView * const view;

    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();
    Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>> test_spec_model;

    std::unordered_set<
        Glib::RefPtr<DiscoveryTestExecutable>,
        std::hash<DiscoveryTestExecutable>,
        DereferencingEqualityFunctor<Glib::RefPtr<DiscoveryTestExecutable>, DiscoveryTestExecutable>
    > discovery_exe_cache;
};

} // namespace optifol

#endif // MANAGETESTSPOPOVER_HPP
