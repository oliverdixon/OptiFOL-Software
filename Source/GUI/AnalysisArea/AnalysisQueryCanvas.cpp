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

using namespace CanvasSupport;

AnalysisQueryCanvas::AnalysisQueryCanvas()
{
    set_draw_func(sigc::mem_fun(*this, &AnalysisQueryCanvas::on_draw));
}

void AnalysisQueryCanvas::add_resolvent(const ProofTreeNode *terminating_node)
{
    add_node(terminating_node);
}

const NodeDrawingAdapter *AnalysisQueryCanvas::add_node(const ProofTreeNode *node)
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
    static constexpr float v_indent = 50;
    static constexpr float h_spacing = 100;
    static constexpr float v_spacing = 80;

    Cairo::FontExtents font_extents;

    ctx->select_font_face("Monospace", Cairo::ToyFontFace::Slant::NORMAL, Cairo::ToyFontFace::Weight::NORMAL);
    ctx->set_font_size(14.0);
    ctx->get_font_extents(font_extents);

    float last_x_pos = 0;

    for (auto& [depth, layer_contents] : nodes)
        for (auto&& [idx, node] : layer_contents | std::views::enumerate) {
            ctx->get_text_extents(node.node_label, node.label_extents);

            node.start.y = v_indent + v_spacing * depth;
            node.centre.y = node.start.y + node.label_extents.height / 2 + rectangle_padding;

            if (depth > 0) {
                assert(node.lhs != nullptr);
                assert(node.rhs != nullptr);
                node.centre.x = node.lhs->centre.x + (node.rhs->centre.x - node.lhs->centre.x) / 2 + rectangle_padding;
            } else {
                node.centre.x = last_x_pos + h_spacing + node.label_extents.width / 2 + rectangle_padding;
                last_x_pos = node.centre.x + node.label_extents.width / 2;
            }

            node.start.x = node.centre.x - node.label_extents.width / 2;

            if (depth > 0) {
                const float advance_y = node.edge_label_lines.size() * font_extents.height + 2 * rectangle_padding;
                node.start.y += advance_y;
                node.centre.y += advance_y;

                const LineSegment lhs_edge(node.centre, node.lhs->centre);
                const LineSegment rhs_edge(node.centre, node.rhs->centre);

                edge_colour.apply(*ctx);
                lhs_edge.draw(*ctx);
                rhs_edge.draw(*ctx);

                if (!node.edge_label_lines.empty()) {
                    const auto lhs_avg = (node.centre.y + node.lhs->centre.y) / 2;
                    const auto rhs_avg = (node.centre.y + node.rhs->centre.y) / 2;

                    const auto [lhs_midpoint, rhs_midpoint] =
                        draw_unifier(*ctx, node.edge_label_lines, Point(node.centre.x, (lhs_avg + rhs_avg) / 2));

                    const LineSegment lhs_intercepting_segment{
                        lhs_edge.trace(node.lhs->centre.y + (node.lhs->centre.y - node.lhs->start.y)),
                        lhs_edge.trace(node.start.y)
                    };

                    const LineSegment rhs_intercepting_segment{
                        rhs_edge.trace(node.rhs->centre.y + (node.rhs->centre.y - node.rhs->start.y)),
                        rhs_edge.trace(node.start.y)
                    };

                    unifier_edge_colour.apply(*ctx);
                    LineSegment(lhs_intercepting_segment.get_midpoint(), lhs_midpoint).draw(*ctx);
                    LineSegment(rhs_intercepting_segment.get_midpoint(), rhs_midpoint).draw(*ctx);
                }
            }
        }

    // Draw the nodes.
    for (const auto& layer : nodes)
        for (const auto& node : layer.second)
            draw_proof_node(*ctx, node);
}

void AnalysisQueryCanvas::draw_proof_node(Cairo::Context &ctx, const NodeDrawingAdapter &node)
{
    Cairo::TextExtents node_text_extents;
    ctx.get_text_extents(node.node_label, node_text_extents);

    const float rect_w = node_text_extents.width + 2 * rectangle_padding;
    const float rect_h = node_text_extents.height + 2 * rectangle_padding;

    // Draw the rectangle body, coloured according to its position in the proof trace.
    if (node.node->get_depth() == 0)
        axiom_node_colour.apply(ctx);
    else if (node.node->observe_node()->get_triviality_state() == Clause::State::TriviallyFalse)
        terminating_node_colour.apply(ctx);
    else
        deduction_node_colour.apply(ctx);

    ctx.rectangle(node.start.x, node.start.y, rect_w, rect_h);
    ctx.fill();

    // Draw the rectangle border
    border_colour.apply(ctx);
    ctx.rectangle(node.start.x, node.start.y, rect_w, rect_h);
    ctx.stroke();

    // Draw text
    text_colour.apply(ctx);
    ctx.move_to(node.start.x - node_text_extents.x_bearing + rectangle_padding,
        node.start.y - node_text_extents.y_bearing + rectangle_padding);
    ctx.show_text(node.node_label);
}

std::pair<Point, Point> AnalysisQueryCanvas::draw_unifier(
        Cairo::Context &ctx, const std::vector<std::string> &unifier_lines, const Point &centre)
{
    Cairo::FontExtents font_extents;
    ctx.get_font_extents(font_extents);
    float max_width = 0;

    for (const auto& line : unifier_lines) {
        Cairo::TextExtents line_extents;
        ctx.get_text_extents(line, line_extents);
        max_width = std::max(max_width, static_cast<float>(line_extents.width));
    }

    const float box_width = max_width + 2 * rectangle_padding;
    const float box_height = unifier_lines.size() * font_extents.height + 2 * rectangle_padding;

    const Point start(centre.x - box_width / 2.0, centre.y - box_height / 2.0 );

    // Write lines inside the box
    text_colour.apply(ctx);
    float cursor_y = start.y + rectangle_padding + font_extents.ascent;
    for (const auto& line : unifier_lines) {
        ctx.move_to(start.x + rectangle_padding, cursor_y);
        ctx.show_text(line);
        cursor_y += font_extents.height;
    }

    return { Point( start.x, centre.y ), Point( start.x + box_width, centre.y ) };
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
