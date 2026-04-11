/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Analysis Query GTK page
 * @author Oliver Dixon
 * @date 2026-01-25
 * @version Development
 */

#ifndef OPTIFOL_ANALYSISQUERY_HPP
#define OPTIFOL_ANALYSISQUERY_HPP

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>

#include "../../Inference/Prover.hpp"
#include "../../Inference/QueryResult.hpp"
#include "AnalysisQueryCanvas.hpp"
#include "FOLLexer.hpp"

namespace optifol
{

/**
 * @class AnalysisQuery
 * @brief A single page in a Gtk::Notebook holding the tab widget Gtk::Label and the scrollable Gtk::TextView.
 * @warning The Gtk::Notebook widget does not own its child pages; to avoid memory access problems, always remove
 *  pages from the Gtk::Notebook before destructing the corresponding AnalysisQuery.
 */
class AnalysisQuery
{
public:
    /**
     * @brief Create a new page for another query on the KB.
     * @param query_name The human-readable string of the query.
     * @param kb_weak A weak reference to the prover instance encapsulating the KB to be queried.
     */
    explicit AnalysisQuery(const Glib::ustring& query_name, Prover& kb_weak);

    /**
     * @brief Append the built page to an arbitrary notebook.
     * @param notebook The notebook to which the page should be appended.
     */
    void add_to_notebook(Gtk::Notebook& notebook);

private:
    /**
     * @brief Check consistency of the current query (in the text box) with the KB. If a contradiction is derived, a
     *  proof trace is generated in the drawing area.
     */
    void execute_query();

    std::vector<std::unique_ptr<Literal>> literal_store;

    Gtk::Label tab_label;
    Gtk::ScrolledWindow scrolled_window;
    AnalysisQueryCanvas drawing_area;
    Gtk::Box outer_box;
    Gtk::Box inner_box;
    Gtk::Entry query_entry;
    Gtk::Button execute_query_button;

    Prover& kb_weak;
    std::unique_ptr<QueryResult> latest_result;

    static std::istringstream lexer_input_stream;
    static FOLLexer lexer;
    static FOLParser parser;
};

} // namespace optifol

#endif // OPTIFOL_ANALYSISQUERY_HPP
