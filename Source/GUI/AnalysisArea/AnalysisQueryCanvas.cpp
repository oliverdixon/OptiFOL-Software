/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the proof trace canvas
 * @author Oliver Dixon
 * @date 2025-02-03
 * @version Development
 */

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

void AnalysisQueryCanvas::replace_proof(const ProofTreeNode *terminating_node)
{
    assert(terminating_node->observe_node()->get_triviality_state() == Clause::State::TriviallyFalse);
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

void AnalysisQueryCanvas::on_draw(const Cairo::RefPtr<Cairo::Context> &ctx, const int width, const int height)
{
    std::ignore = width;
    std::ignore = height;

    if (nodes.empty())
        return;

    Cairo::FontExtents font_extents;

    ctx->select_font_face("Monospace", Cairo::ToyFontFace::Slant::NORMAL, Cairo::ToyFontFace::Weight::NORMAL);
    ctx->set_font_size(14.0);
    ctx->get_font_extents(font_extents);

    float last_x_pos = 0;

    // Draw edges and prepare the node coordinates and Cairo text extents.
    for (auto& [depth, layer_contents] : nodes)
        for (auto&& node : layer_contents)
            last_x_pos = draw_edge(*ctx, node, last_x_pos, font_extents);

    int maximised_width = 0;
    int maximised_height = 0;

    // Draw the first layer of nodes (from which the north-eastern-most point can be deduced).
    for (const auto& node : nodes.begin()->second) {
        const auto end_point = draw_proof_node(*ctx, node);
        maximised_width = std::max(maximised_width, static_cast<int>(end_point.x));
    }

    // Draw the middle section of nodes.
    for (const auto& layer : nodes | std::views::drop(1) | std::views::take(nodes.size() - 2))
        for (const auto& node : layer.second)
            draw_proof_node(*ctx, node);

    // Draw the final layer of nodes (from which the south-eastern-most point can be deduced).
    const auto& last_layer_it = --nodes.end();
    if (last_layer_it != nodes.begin())
        for (const auto& node : last_layer_it->second) {
            const auto end_point = draw_proof_node(*ctx, node);
            maximised_height = std::max(maximised_height, static_cast<int>(end_point.y));
        }

    content_width = maximised_width + static_cast<int>(h_spacing);
    content_height = maximised_height + static_cast<int>(v_spacing);
    set_size_request(content_width, content_height);
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive.
Point AnalysisQueryCanvas::draw_proof_node(Cairo::Context &ctx, const NodeDrawingAdapter &node)
{
    assert(node.start.x < node.centre.x);
    assert(node.start.y < node.centre.y);
    assert(node.label_extents.width > 0);
    assert(node.label_extents.height > 0);

    // Draw the rectangle body, coloured according to its position in the proof trace.
    if (node.node->get_depth() == 0)
        axiom_node_colour.apply(ctx);
    else if (node.node->observe_node()->get_triviality_state() == Clause::State::TriviallyFalse)
        terminating_node_colour.apply(ctx);
    else
        deduction_node_colour.apply(ctx);

    // The rectangle requires space to house the text, plus padding along all sides.
    const float rect_w = static_cast<float>(node.label_extents.width) + 2 * rectangle_padding;
    const float rect_h = static_cast<float>(node.label_extents.height) + 2 * rectangle_padding;

    ctx.rectangle(node.start.x, node.start.y, rect_w, rect_h);
    ctx.fill();

    // Draw the rectangle border.
    border_colour.apply(ctx);
    ctx.rectangle(node.start.x, node.start.y, rect_w, rect_h);
    ctx.stroke();

    // Draw the text inside the rectangle.
    text_colour.apply(ctx);
    ctx.move_to(
        node.start.x - node.label_extents.x_bearing + rectangle_padding,
        node.start.y - node.label_extents.y_bearing + rectangle_padding
    );

    ctx.show_text(node.node_label);

    // Report the south-eastern-most point drawn.
    return { node.centre.x + rect_w / 2.0f, node.centre.y + rect_h / 2.0f };
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive.
void AnalysisQueryCanvas::draw_unifier(Cairo::Context &ctx, const NodeDrawingAdapter &node, const LineSegment &lhs_edge,
        const LineSegment &rhs_edge, const Cairo::FontExtents &font_extents)
{
    assert(node.start.x < node.centre.x);
    assert(node.start.y < node.centre.y);

    /*
     * Draw the unifier text entries (each representing a substitution made in the resolution clause) centred above the
     * resolvent, aligned horizontally with the average of the distances to either parent.
     */
    const auto lhs_avg = (node.centre.y + node.lhs->centre.y) / 2;
    const auto rhs_avg = (node.centre.y + node.rhs->centre.y) / 2;

    const auto [lhs_midpoint, rhs_midpoint] =
        draw_unifier_entries(ctx, node.edge_label_lines, Point(node.centre.x, (lhs_avg + rhs_avg) / 2), font_extents);

    /*
     * Compute the endpoints of the visible edges, chopping off endpoints that are hidden behind nodes. This is required
     * for computing the midpoint of the hypotenuse that connects the resolvent with each parent.
     */
    const LineSegment lhs_intercepting_segment{
        lhs_edge.trace(node.lhs->centre.y + (node.lhs->centre.y - node.lhs->start.y)),
        lhs_edge.trace(node.start.y)
    };

    const LineSegment rhs_intercepting_segment{
        rhs_edge.trace(node.rhs->centre.y + (node.rhs->centre.y - node.rhs->start.y)),
        rhs_edge.trace(node.start.y)
    };

    // Draw lines between the midpoints of the visible parent-child edges and the centre-aligned unifier line block.

    unifier_edge_colour.apply(ctx);
    LineSegment(lhs_intercepting_segment.get_midpoint(), lhs_midpoint).draw(ctx);
    LineSegment(rhs_intercepting_segment.get_midpoint(), rhs_midpoint).draw(ctx);
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive.
float AnalysisQueryCanvas::draw_edge(Cairo::Context &ctx, CanvasSupport::NodeDrawingAdapter &node, float last_x_pos,
        const Cairo::FontExtents &font_extents)
{
    // Compute the coordinates and the text extents of the node.
    ctx.get_text_extents(node.node_label, node.label_extents);

    const auto depth = node.node->get_depth();
    node.start.y = v_spacing * (static_cast<float>(depth) + 1);
    node.centre.y = node.start.y + static_cast<float>(node.label_extents.height) / 2.0f + rectangle_padding;

    if (depth > 0) {
        assert(node.lhs != nullptr);
        assert(node.rhs != nullptr);
        node.centre.x = node.lhs->centre.x + (node.rhs->centre.x - node.lhs->centre.x) / 2 + rectangle_padding;
    } else {
        node.centre.x = last_x_pos + h_spacing + static_cast<float>(node.label_extents.width) / 2 + rectangle_padding;
        last_x_pos = node.centre.x + static_cast<float>(node.label_extents.width) / 2;
    }

    node.start.x = node.centre.x - static_cast<float>(node.label_extents.width) / 2;

    if (depth > 0) {
        /*
         * If we're not drawing an axiom (i.e. a resolvent) there will be two edges to each parent (LHS and RHS). There
         * may also be an applicable multi-line unifier text block to render at the midpoint, which will require the
         * resolvent to be moved down proportional to the number of unification entries.
         */
        const float advance_y = static_cast<float>(node.edge_label_lines.size()) *
            static_cast<float>(font_extents.height) + 2.0f * rectangle_padding;
        node.start.y += advance_y;
        node.centre.y += advance_y;

        // Draw the edges from each parent to the resolvent.
        const LineSegment lhs_edge(node.centre, node.lhs->centre);
        const LineSegment rhs_edge(node.centre, node.rhs->centre);

        edge_colour.apply(ctx);
        lhs_edge.draw(ctx);
        rhs_edge.draw(ctx);

        if (!node.edge_label_lines.empty())
            // If applicable, draw the unifier and connecting lines.
            draw_unifier(ctx, node, lhs_edge, rhs_edge, font_extents);
    }

    return last_x_pos;
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive.
std::pair<Point, Point> AnalysisQueryCanvas::draw_unifier_entries(Cairo::Context &ctx,
        const std::vector<std::string> &unifier_lines, const Point &centre, const Cairo::FontExtents &font_extents)
{
    // Determine the box width according to the maximum line length of the rendered unifier entries.
    float max_width = 0;
    for (const auto& line : unifier_lines) {
        Cairo::TextExtents line_extents;
        ctx.get_text_extents(line, line_extents);
        max_width = std::max(max_width, static_cast<float>(line_extents.width));
    }

    const float box_width = max_width + 2 * rectangle_padding;
    const float box_height = static_cast<float>(unifier_lines.size()) * static_cast<float>(font_extents.height) +
        2 * rectangle_padding;

    const Point start{
        static_cast<float>(centre.x - box_width / 2.0),
        static_cast<float>(centre.y - box_height / 2.0)
    };

    // Write lines inside the box, incrementing the cursor by the baseline skip extent.
    text_colour.apply(ctx);
    float cursor_y = start.y + rectangle_padding + static_cast<float>(font_extents.ascent);
    for (const auto& line : unifier_lines) {
        ctx.move_to(start.x + rectangle_padding, cursor_y);
        ctx.show_text(line);
        cursor_y += static_cast<float>(font_extents.height);
    }

    // Report the midpoints of the vertical bounding lines.
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
