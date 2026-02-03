/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Collection of graphics-related helper structs for the AnalysisQueryCanvas
 * @author Oliver Dixon
 * @date 2025-02-03
 * @version Development
 */

#ifndef OPTIFOL_CANVASSUPPORT_HPP
#define OPTIFOL_CANVASSUPPORT_HPP

#include <cassert>
#include <cairomm/context.h>

#include "../../Inference/ProofTreeNode.hpp"

namespace optifol::CanvasSupport
{

/**
 * @struct NodeColour
 * @brief A POD RGB colour representation that can be applied to a Cairo::Context.
 */
struct NodeColour
{

    /**
     * @brief The red component, normalised in the range [0, 1].
     */
    float red;

    /**
     * @brief The green component, normalised in the range [0, 1].
     */
    float green;

    /**
     * @brief The green component, normalised in the range [0, 1].
     */
    float blue;

    /**
     * @brief Set the given Cairo::Context to use the colour.
     * @param ctx The target Cairo::Context.
     */
    void apply(Cairo::Context& ctx) const
    {
        ctx.set_source_rgb(red, green, blue);
    }
};

constexpr NodeColour axiom_node_colour{234.0f / 255, 207.0f / 255, 193.0f / 255};
constexpr NodeColour terminating_node_colour{178.0f / 255, 210.0f / 255, 226.0f / 255};
constexpr NodeColour deduction_node_colour{0.95f, 0.95f, 0.95f};
constexpr NodeColour text_colour{0.0f, 0.0f, 0.0f};
constexpr NodeColour border_colour{0.2f, 0.2f, 0.2f};
constexpr NodeColour edge_colour{0.0f, 0.0f, 0.0f};
constexpr NodeColour unifier_edge_colour{0.7f, 0.7f, 0.7f};

/**
 * @struct CanvasDrawable
 * @brief A type that is drawable to a Cairo::Context canvas.
 */
struct CanvasDrawable
{
    /**
     * @brief Destruct the CanvasDrawable.
     */
    virtual ~CanvasDrawable() = default;

    /**
     * @brief Draw the CanvasDrawable item in the colour currently selected by the Cairo::Context.
     * @param ctx The target canvas.
     */
    virtual void draw(Cairo::Context& ctx) const = 0;

    /**
     * @brief Draw the CanvasDrawable item in the given colour.
     * @param ctx The target canvas.
     * @param node_colour The normalised RGB colour.
     */
    void draw(Cairo::Context& ctx, const NodeColour& node_colour) const
    {
        node_colour.apply(ctx);
        draw(ctx);
    }
};

/**
 * @struct Point
 * @brief A CanvasDrawable two-dimensional point on a Cairo::Context canvas.
 */
struct Point : CanvasDrawable
{
    /**
     * @brief Construct the origin point.
     */
    Point() = default;

    /**
     * @brief Construct the point at the given coordinates.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     */
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

/**
 * @struct LineSegment
 * @brief A line segment (a line with two fixed endpoints) on a Cairo::Context canvas.
 */
struct LineSegment : CanvasDrawable
{
    /**
     * @brief Construct the line segment with the given endpoints.
     * @param p1 The first endpoint.
     * @param p2 The second endpoint.
     */
    LineSegment(const Point& p1, const Point &p2) :
        endpoint_1(p1),
        endpoint_2(p2),
        gradient(p1.x == p2.x ? INFINITY : (p1.y - p2.y) / (p1.x - p2.x))
    { }

    /**
     * @brief Determine the X coordinate on the non-horizontal line for the given Y coordinate.
     * @param y The Y position on the line for which the corresponding X is sought.
     * @throws std::runtime_error The line was horizontal, hence there is no unique X for a given Y.
     * @return The Point on the LineSegment at the given Y position.
     * @note This function does not check that the returned pair is actually on the LineSegment. The API user should
     *  verify that the given Y is within the bounds of the LineSegment endpoints.
     */
    [[nodiscard]] Point trace(const float y) const
    {
        switch (std::fpclassify(gradient)) {
        case FP_INFINITE:
            // The line is vertical.
            return { endpoint_1.x, y };
        case FP_ZERO:
            // The line is horizontal.
            throw std::runtime_error("There is no unique X coordinate for a given Y on a horizontal line segment.");
        default:
            return { endpoint_1.x + (y - endpoint_1.y) / gradient, y };
        }
    }

    /**
     * @brief Gets the midpoint of the line.
     * @return The midpoint of the LineSegment.
     */
    [[nodiscard]] Point get_midpoint() const noexcept
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

/**
 * @struct NodeDrawingAdapter
 * @brief A transparent cache of drawing information related to a ProofTreeNode, for direct use by AnalysisQueryCanvas.
 */
struct NodeDrawingAdapter
{
    /**
     * @brief Construct a new adapter for the given node, with optional LHS and RHS parents.
     * @param node The self ProofTreeNode, typically a Resolvent, but sometimes a Clause (for axioms in the proof).
     * @param lhs An optional parent on the LHS.
     * @param rhs An optional parent on the RHS.
     * @note The given node must have exactly zero or exactly two parents; this is enforced at runtime.
     */
    NodeDrawingAdapter(const ProofTreeNode * const node, const NodeDrawingAdapter * const lhs,
            const NodeDrawingAdapter * const rhs) :
        node_label(generate_label(*node)),
        node(node),
        lhs(lhs),
        rhs(rhs)
    {
        assert((!lhs && !rhs) || (lhs && rhs));

        if (node->observe_edge().has_value())
            // If applicable, cache the delimeted lines in the unifier text block entry.
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
    /**
     * @brief Stringify the given ISerialisable.
     * @param node The node to serialise.
     * @return The serialised string produced from the node.
     */
    static std::string generate_label(const ISerialisable& node)
    {
        std::ostringstream oss;
        oss << node;
        return oss.str();
    }
};

} // namespace optifol::CanvasSupport

#endif // OPTIFOL_CANVASSUPPORT_HPP
