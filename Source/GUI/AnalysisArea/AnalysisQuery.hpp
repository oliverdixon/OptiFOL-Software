/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/25/26.
//

#ifndef OPTIFOL_ANALYSISQUERY_HPP
#define OPTIFOL_ANALYSISQUERY_HPP

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>

#include "../../Inference/KnowledgeBase.hpp"
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
    explicit AnalysisQuery(const Glib::ustring& query_name, KnowledgeBase& kb_weak);

    void add_to_notebook(Gtk::Notebook& notebook);

private:
    void execute_query();

    std::vector<std::unique_ptr<Literal>> literal_store;

    Gtk::Label tab_label;
    Gtk::ScrolledWindow scrolled_window;
    AnalysisQueryCanvas drawing_area;
    Gtk::Box outer_box;
    Gtk::Box inner_box;
    Gtk::Entry query_entry;
    Gtk::Button execute_query_button;

    KnowledgeBase& kb_weak;

    static std::istringstream lexer_input_stream;
    static FOLLexer lexer;
    static FOLParser parser;
};

} // namespace optifol

#endif // OPTIFOL_ANALYSISQUERY_HPP
