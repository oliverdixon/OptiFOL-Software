/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Analysis Query GTK page
 * @author Oliver Dixon
 * @date 2026-01-25
 * @version Development
 */

#include "AnalysisQuery.hpp"

#include <cassert>

#include "../../Exceptions/SemanticException.hpp"

namespace optifol
{

std::istringstream AnalysisQuery::lexer_input_stream;

/*
 * TODO: this construction is possibly undefined due to std::cerr. But for the real case, we'll use a custom error
 *  handler that can be statically initialised in the Analysis Manager, so this is OK for development.
 */
FOLLexer AnalysisQuery::lexer{AnalysisQuery::lexer_input_stream, std::cerr};
FOLParser AnalysisQuery::parser{&AnalysisQuery::lexer};

AnalysisQuery::AnalysisQuery(const Glib::ustring &query_name, Prover& kb_weak) :
    kb_weak(kb_weak)
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
    if (query_entry.get_text().empty())
        return;

    try {
        lexer_input_stream.str(query_entry.get_text());
        parser.parse();
    } catch (const ParseError& parse_error) {
        // TODO log
        assert(0);
        return;
    }

    try {
        latest_result = std::make_unique<QueryResult>(kb_weak.ask(parser.retrieve_sentence()));
        if (latest_result->terminating_resolvent != nullptr)
            drawing_area.replace_proof(latest_result->terminating_resolvent);
    } catch (const SemanticException& semantic_exception) {
        // TODO log
        assert(0);
        return;
    }

    drawing_area.queue_draw();
}

} // namespace optifol
