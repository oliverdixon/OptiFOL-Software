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

namespace optifol
{

class AnalysisQueryCanvas : public Gtk::DrawingArea
{
    struct NodeDrawingAdapter
    {
        NodeDrawingAdapter(const ProofTreeNode * const node, const NodeDrawingAdapter * const lhs,
                const NodeDrawingAdapter * const rhs) :
            label(generate_label(*node)),
            node(node),
            lhs(lhs),
            rhs(rhs)
        { }

        float start_x = 0;
        float start_y = 0;
        float centre_x = 0;
        float centre_y = 0;

        const std::string label;
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

public:
    AnalysisQueryCanvas();

    void add_resolvent(const ProofTreeNode *terminating_node);

protected:
    const NodeDrawingAdapter * add_node(const ProofTreeNode *node);

    void on_draw(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height);

    Gtk::SizeRequestMode get_request_mode_vfunc() const override;

    void measure_vfunc(Gtk::Orientation orientation, int for_size, int &minimum, int &natural, int &minimum_baseline,
        int &natural_baseline) const override;

private:
    struct NodeColour
    {
        float red;
        float green;
        float blue;
    };

    static constexpr NodeColour axiom_colouring{234.0 / 255, 207.0 / 255, 193.0 / 255};
    static constexpr NodeColour terminating_colouring{178.0 / 255, 210.0 / 255, 226.0 / 255};
    static constexpr NodeColour regular_colouring{0.95, 0.95, 0.95};

    std::map<unsigned int, std::deque<NodeDrawingAdapter>> nodes;

    int content_width = 500; // TODO measure content properly
    int content_height = 500;
};

} // namespace optifol

#endif // OPTIFOL_ANALYSISQUERYCANVAS_HPP
