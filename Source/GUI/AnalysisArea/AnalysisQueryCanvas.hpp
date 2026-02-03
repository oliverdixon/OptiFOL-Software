/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/25/26.
//

#ifndef OPTIFOL_ANALYSISQUERYCANVAS_HPP
#define OPTIFOL_ANALYSISQUERYCANVAS_HPP

#include <gtkmm/drawingarea.h>

#include "../../Inference/Resolvent.hpp"
#include "CanvasSupport.hpp"

namespace optifol
{

class AnalysisQueryCanvas : public Gtk::DrawingArea
{
public:
    AnalysisQueryCanvas();

    void add_resolvent(const ProofTreeNode *terminating_node);

    Gtk::SizeRequestMode get_request_mode_vfunc() const override;

    void measure_vfunc(Gtk::Orientation orientation, int for_size, int &minimum, int &natural, int &minimum_baseline,
        int &natural_baseline) const override;

private:


    const CanvasSupport::NodeDrawingAdapter * add_node(const ProofTreeNode *node);

    void on_draw(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height);

    static void draw_proof_node(Cairo::Context &ctx, const CanvasSupport::NodeDrawingAdapter &node);

    static std::pair<CanvasSupport::Point, CanvasSupport::Point> draw_unifier(
            Cairo::Context &ctx, const std::vector<std::string> &unifier_lines, const CanvasSupport::Point &centre);

    static constexpr float rectangle_padding = 6;

    std::map<unsigned int, std::deque<CanvasSupport::NodeDrawingAdapter>> nodes;

    int content_width = 500; // TODO measure content properly
    int content_height = 500;
};

} // namespace optifol

#endif // OPTIFOL_ANALYSISQUERYCANVAS_HPP
