/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/25/26.
//

#include "AnalysisQuery.hpp"

namespace optifol
{

AnalysisQuery::AnalysisQuery(const Glib::ustring &query_name)
{
    execute_query_button.signal_clicked().connect(sigc::mem_fun(*this, &AnalysisQuery::execute_query));
    tab_label.set_text(query_name);

    // The inner box contains the query entry area
    query_entry.set_placeholder_text("Query");
    query_entry.set_hexpand();

    execute_query_button.set_icon_name("edit-find-symbolic");
    execute_query_button.set_tooltip_text("Execute Query");

    inner_box.set_hexpand();
    inner_box.add_css_class("optifol_boxed");
    inner_box.append(query_entry);
    inner_box.append(execute_query_button);

    // The scrolled window encapsulates the Cairo drawing surface
    drawing_area.set_hexpand(false);
    drawing_area.set_vexpand(false);
    scrolled_window.set_vexpand();
    scrolled_window.set_child(drawing_area);

    // The outer box is vertical-oriented and stacks the query entry on top of the scrollable drawing area.
    outer_box.set_orientation(Gtk::Orientation::VERTICAL);
    outer_box.append(inner_box);
    outer_box.append(scrolled_window);
}

void AnalysisQuery::add_to_notebook(Gtk::Notebook &notebook)
{
    notebook.append_page(outer_box, tab_label);
}

void AnalysisQuery::execute_query()
{
    // TODO: test resolvent

    literal_store.push_back(Literal::build("P"));
    literal_store.push_back(Literal::build("Q"));
    literal_store.push_back(Literal::build("Q"));
    literal_store.push_back(Literal::build("S"));

    Clause lhs_clause{literal_store[0].get(), literal_store[1].get()};
    Clause rhs_clause{literal_store[2].get(), literal_store[3].get()};
    Clause resolution{literal_store[0].get(), literal_store[3].get()};

    const Resolvent resolvent{std::move(lhs_clause), std::move(rhs_clause), {}, std::move(resolution)};

    drawing_area.add_resolvent(resolvent);

    drawing_area.queue_draw();
}

} // namespace optifol
