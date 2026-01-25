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
public:
    AnalysisQueryCanvas();

    void add_resolvent(const Resolvent& resolvent);

protected:
    void on_draw(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height);

    Gtk::SizeRequestMode get_request_mode_vfunc() const override;

    void measure_vfunc(Gtk::Orientation orientation, int for_size, int &minimum, int &natural, int &minimum_baseline,
        int &natural_baseline) const override;

private:
    struct Node
    {
        std::string formula;
        const Node * lhs_parent;
        const Node * rhs_parent;
    };

    std::vector<Node> nodes;

    int content_width = 500; // TODO measure content properly
    int content_height = 500;
};

} // namespace optifol

#endif // OPTIFOL_ANALYSISQUERYCANVAS_HPP
