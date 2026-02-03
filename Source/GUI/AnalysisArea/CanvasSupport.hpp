/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 2/3/26.
//

#ifndef OPTIFOL_CANVASSUPPORT_HPP
#define OPTIFOL_CANVASSUPPORT_HPP

#include <cairomm/context.h>

#include "../../Inference/ProofTreeNode.hpp"

namespace optifol::CanvasSupport
{

struct NodeColour
{
    float red;
    float green;
    float blue;

    void apply(Cairo::Context& ctx) const
    {
        ctx.set_source_rgb(red, green, blue);
    }
};

constexpr NodeColour axiom_node_colour{234.0 / 255, 207.0 / 255, 193.0 / 255};
constexpr NodeColour terminating_node_colour{178.0 / 255, 210.0 / 255, 226.0 / 255};
constexpr NodeColour deduction_node_colour{0.95, 0.95, 0.95};
constexpr NodeColour text_colour{0, 0, 0};
constexpr NodeColour border_colour{0.2, 0.2, 0.2};
constexpr NodeColour edge_colour{0, 0, 0};
constexpr NodeColour unifier_edge_colour{.7, .7, .7};

struct CanvasDrawable
{
    virtual ~CanvasDrawable() = default;

    virtual void draw(Cairo::Context& ctx) const = 0;

    void draw(Cairo::Context& ctx, const NodeColour& node_colour) const
    {
        node_colour.apply(ctx);
        draw(ctx);
    }
};

struct Point : CanvasDrawable
{
    Point() = default;

    Point(const float x, const float y) :
        x(x),
        y(y)
    { }

    void draw(Cairo::Context& ctx) const override
    {
        ctx.arc(x, y, 5, 0, 2 * M_PI);
        ctx.stroke();
    }

    float x = 0;
    float y = 0;
};

struct LineSegment : CanvasDrawable
{
    LineSegment(const Point& p1, const Point &p2) :
        endpoint_1(p1),
        endpoint_2(p2),
        gradient((p1.y - p2.y) / (p1.x - p2.x)) // TODO vertical and horizontal lines
    { }

    Point trace(const float y) const noexcept
    {
        return { endpoint_1.x + (y - endpoint_1.y) / gradient, y };
    }

    Point get_midpoint() const noexcept
    {
        return { (endpoint_1.x + endpoint_2.x) / 2, (endpoint_1.y + endpoint_2.y) / 2 };
    }

    void draw(Cairo::Context& ctx) const override
    {
        ctx.move_to(endpoint_1.x, endpoint_1.y);
        ctx.line_to(endpoint_2.x, endpoint_2.y);
        ctx.stroke();
    }

    const Point endpoint_1;
    const Point endpoint_2;
    const float gradient;
};

struct NodeDrawingAdapter
{
    NodeDrawingAdapter(const ProofTreeNode * const node, const NodeDrawingAdapter * const lhs,
            const NodeDrawingAdapter * const rhs) :
        node_label(generate_label(*node)),
        node(node),
        lhs(lhs),
        rhs(rhs)
    {
        if (node->observe_edge().has_value())
            edge_label_lines = std::move(node->observe_edge().value()->split_serialise());
    }

    Point start;
    Point centre;
    Cairo::TextExtents label_extents{};

    const std::string node_label;
    std::vector<std::string> edge_label_lines;
    const ProofTreeNode * node;
    const NodeDrawingAdapter * lhs = nullptr;
    const NodeDrawingAdapter * rhs = nullptr;

private:
    static std::string generate_label(const ISerialisable& node)
    {
        std::ostringstream oss; // TODO efficiency
        oss << node;
        return oss.str();
    }
};

} // namespace optifol::CanvasSupport

#endif // OPTIFOL_CANVASSUPPORT_HPP
