/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/25/26.
//

#include "AnalysisQueryCanvas.hpp"

#include <cassert>
#include <ranges>

namespace optifol
{

AnalysisQueryCanvas::AnalysisQueryCanvas()
{
    set_draw_func(sigc::mem_fun(*this, &AnalysisQueryCanvas::on_draw));
}

void AnalysisQueryCanvas::add_resolvent(const ProofTreeNode *terminating_node)
{
    add_node(terminating_node);
}

const AnalysisQueryCanvas::NodeDrawingAdapter *AnalysisQueryCanvas::add_node(const ProofTreeNode *node)
{
    const NodeDrawingAdapter * lhs_adapter = nullptr;
    const NodeDrawingAdapter * rhs_adapter = nullptr;

    if (node->get_depth() > 0) {
        if (node->observe_lhs_parent() != nullptr)
            lhs_adapter = add_node(node->observe_lhs_parent());

        if (node->observe_rhs_parent() != nullptr)
            rhs_adapter = add_node(node->observe_rhs_parent());
    }

    return &nodes[node->get_depth()].emplace_back(node, lhs_adapter, rhs_adapter);
}

void AnalysisQueryCanvas::on_draw(const Cairo::RefPtr<Cairo::Context> &ctx, int width, int height)
{
    static constexpr float rectangle_padding = 6;
    static constexpr float h_indent = 50;
    static constexpr float v_indent = 50;
    static constexpr float h_spacing = 100;
    static constexpr float v_spacing = 80;

    Cairo::TextExtents text_extents;

    ctx->select_font_face("Monospace", Cairo::ToyFontFace::Slant::NORMAL, Cairo::ToyFontFace::Weight::NORMAL);
    ctx->set_font_size(14.0);

    float last_x_pos = h_indent;

    for (auto& [depth, layer_contents] : nodes)
        for (auto&& [idx, node] : layer_contents | std::views::enumerate) {
            ctx->get_text_extents(node.label, text_extents);

            node.start_y = v_indent + v_spacing * depth;
            node.centre_y = node.start_y + text_extents.height / 2;

            if (depth > 0) {
                assert(node.lhs != nullptr);
                assert(node.rhs != nullptr);
                node.centre_x = node.lhs->centre_x + (node.rhs->centre_x - node.lhs->centre_x) / 2;
            } else {
                node.centre_x = last_x_pos + h_spacing + text_extents.width / 2;
                last_x_pos = node.centre_x + text_extents.width / 2;
            }

            node.start_x = node.centre_x - text_extents.width / 2;

            if (depth > 0) {
                ctx->move_to(node.lhs->centre_x, node.lhs->centre_y);
                ctx->line_to(node.centre_x, node.centre_y);
                ctx->stroke();

                ctx->move_to(node.rhs->centre_x, node.rhs->centre_y);
                ctx->line_to(node.centre_x, node.centre_y);
                ctx->stroke();
            }
        }

    // Draw the nodes.
    for (const auto& [depth, layer_contents] : nodes)
        for (const auto& [idx, node] : layer_contents | std::views::enumerate) {
            ctx->get_text_extents(node.label, text_extents);

            const float rect_x = node.start_x + text_extents.x_bearing - rectangle_padding;
            const float rect_y = node.start_y + text_extents.y_bearing - rectangle_padding;
            const float rect_w = text_extents.width + 2 * rectangle_padding;
            const float rect_h = text_extents.height + 2 * rectangle_padding;

            // Draw the rectangle body
            if (depth == 0)
                ctx->set_source_rgb(axiom_colouring.red, axiom_colouring.green, axiom_colouring.blue);
            else if (node.node->observe_substance()->get_triviality_state() == Clause::State::TriviallyFalse)
                ctx->set_source_rgb(terminating_colouring.red, terminating_colouring.green, terminating_colouring.blue);
            else
                ctx->set_source_rgb(regular_colouring.red, regular_colouring.green, regular_colouring.blue);

            ctx->rectangle(rect_x, rect_y, rect_w, rect_h);
            ctx->fill();

            // Draw the rectangle border
            ctx->set_source_rgb(0.2, 0.2, 0.2);
            ctx->rectangle(rect_x, rect_y, rect_w, rect_h);
            ctx->stroke();

            // Draw text
            ctx->set_source_rgb(0.0, 0.0, 0.0);
            ctx->move_to(node.start_x, node.start_y);
            ctx->show_text(node.label);
        }
}

Gtk::SizeRequestMode AnalysisQueryCanvas::get_request_mode_vfunc() const
{
    return Gtk::SizeRequestMode::CONSTANT_SIZE;
}

void AnalysisQueryCanvas::measure_vfunc(const Gtk::Orientation orientation, const int for_size, int &minimum,
    int &natural, int &minimum_baseline, int &natural_baseline) const
{
    if (orientation == Gtk::Orientation::HORIZONTAL) {
        minimum = content_width;
        natural = content_width;
    } else {
        minimum = content_height;
        natural = content_height;
    }
}

} // namespace optifol
