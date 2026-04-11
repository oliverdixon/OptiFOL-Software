/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the proof trace canvas
 * @author Oliver Dixon
 * @date 2025-02-03
 * @version Development
 */

#ifndef OPTIFOL_ANALYSISQUERYCANVAS_HPP
#define OPTIFOL_ANALYSISQUERYCANVAS_HPP

#include <gtkmm/drawingarea.h>

#include "../../Inference/Resolvent.hpp"
#include "CanvasSupport.hpp"

namespace optifol
{

/**
 * @class AnalysisQueryCanvas
 * @brief The canvas for rendering resolution proof trees with Cairo.
 */
class AnalysisQueryCanvas : public Gtk::DrawingArea
{
public:
    /**
     * @brief Construct an empty canvas.
     */
    AnalysisQueryCanvas();

    /**
     * @brief Replace any existing proofs with a new proof, terminating by the given node in the tree.
     * @param terminating_node The derived contradiction, representing the deepest element in the execution
     * trace.
     */
    void replace_proof(const ProofTreeNode *terminating_node);

    Gtk::SizeRequestMode get_request_mode_vfunc() const override;

    void measure_vfunc(Gtk::Orientation orientation, int for_size, int &minimum, int &natural,
            int &minimum_baseline, int &natural_baseline) const override;

private:
    /**
     * @brief Recursively wrap the given ProofTreeNode (and its parents) in drawing adapters and add to the
     * store.
     * @param node The root node to add.
     * @return An observing pointer to the registered root node.
     */
    const CanvasSupport::NodeDrawingAdapter *add_node(const ProofTreeNode *node);

    /**
     * @brief Redraws a visualisation of the current proof trace to the given Cairo::Context.
     * @param ctx The Cairo::Context to which the proof trace render should be drawn.
     * @param width Not used, required by the GTKmm interface.
     * @param height Not used, required by the GTKmm interface.
     */
    void on_draw(const Cairo::RefPtr<Cairo::Context> &ctx, int width, int height);

    /**
     * @brief Draw a single ProofTreeNode encapsulated by the given NodeDrawingAdapter.
     * @details For any type of ProofTreeNode, this renderer draws a bordered box containing a single
     *  monospace-formatted text representing the serialisation of the node. This represents a Clause in the
     * proof.
     * @param ctx The Cairo::Context to which the node should be drawn.
     * @param node The NodeDrawingAdapter containing the node to be drawn.
     * @return The Point at the south-eastern-most (i.e. most extreme) point drawn by the function.
     * @pre The X coordinate of the NodeDrawingAdapter has been properly initialised.
     * @pre The Y coordinate of the NodeDrawingAdapter has been properly initialised.
     * @pre The text extents of the label has been properly initialised.
     */
    static CanvasSupport::Point draw_proof_node(
            Cairo::Context &ctx, const CanvasSupport::NodeDrawingAdapter &node);

    /**
     * @brief Render a unifier on an edge, with adjoining lines indicating the source Clauses and Resolvent.
     * @param ctx The Cairo::Context to which the unifier and lines should be drawn.
     * @param node The NodeDrawingAdapter encapsulating the Resolvent with the Unifier to be drawn.
     * @param lhs_edge The LineSegment joining the centre of the Resolvent node with the centre of its LHS
     * parent.
     * @param rhs_edge The LineSegment joining the centre of the Resolvent node with the centre of its RHS
     * parent.
     * @param font_extents The precomputed extents of the font, required for baseline skip calculations.
     * @pre The X coordinate of the NodeDrawingAdapter has been properly initialised.
     * @pre The Y coordinate of the NodeDrawingAdapter has been properly initialised.
     */
    static void draw_unifier(Cairo::Context &ctx, const CanvasSupport::NodeDrawingAdapter &node,
            const CanvasSupport::LineSegment &lhs_edge, const CanvasSupport::LineSegment &rhs_edge,
            const Cairo::FontExtents &font_extents);

    /**
     * @brief Prepares a node, and, if applicable, draws its parent-child edges and unifier.
     * @param ctx The Cairo::Context to which the edge and unifier should be drawn.
     * @param node The node to be prepared.
     * @param last_x_pos The trailing X position of the last node at the same depth.
     * @param font_extents The precomputed extents of the font, required for baseline skip calculations.
     * @return The trailing X position of the rendered node.
     */
    static float draw_edge(Cairo::Context &ctx, CanvasSupport::NodeDrawingAdapter &node, float last_x_pos,
            const Cairo::FontExtents &font_extents);

    /**
     * @brief Render the unifier entries, with one substitution per line, at the given centre point.
     * @param ctx The Cairo::Context to which the text should be rendered.
     * @param unifier_lines The serialised unifier lines.
     * @param centre The centre point of the bounding box in which the text is rendered.
     * @param font_extents The precomputed extents of the font, required for baseline skip calculations.
     * @return The midpoints of the LHS and RHS vertical boundaries of the bounding box.
     */
    static std::pair<CanvasSupport::Point, CanvasSupport::Point> draw_unifier_entries(Cairo::Context &ctx,
            const std::vector<std::string> &unifier_lines, const CanvasSupport::Point &centre,
            const Cairo::FontExtents &font_extents);

    static constexpr float rectangle_padding = 6;
    static constexpr float h_spacing = 100;
    static constexpr float v_spacing = 80;

    std::map<unsigned int, std::deque<CanvasSupport::NodeDrawingAdapter>> nodes;

    int content_width = 0;
    int content_height = 0;
};

} // namespace optifol

#endif // OPTIFOL_ANALYSISQUERYCANVAS_HPP
