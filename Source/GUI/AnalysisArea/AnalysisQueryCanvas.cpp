/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/25/26.
//

#include "AnalysisQueryCanvas.hpp"

#include <ranges>

namespace optifol
{

AnalysisQueryCanvas::AnalysisQueryCanvas()
{
    set_draw_func(sigc::mem_fun(*this, &AnalysisQueryCanvas::on_draw));
}

void AnalysisQueryCanvas::add_resolvent(const Resolvent &resolvent)
{
    std::ostringstream oss;
    oss << resolvent.observe_resolution();

    nodes.emplace_back(oss.str(), nullptr, nullptr); // TODO how to find parents?
}

void AnalysisQueryCanvas::on_draw(const Cairo::RefPtr<Cairo::Context> &ctx, int width, int height)
{
    // coordinates for the center of the window
    const int xc = width / 2;
    const int yc = height / 2;

    static constexpr int padding = 6.0;

    ctx->select_font_face("Sans", Cairo::ToyFontFace::Slant::NORMAL, Cairo::ToyFontFace::Weight::NORMAL);
    ctx->set_font_size(14.0);

    for (const auto [node_idx, resolvent] : std::views::enumerate(nodes)) {
        // Measure text
        Cairo::TextExtents extents;
        ctx->get_text_extents(resolvent.formula, extents);

        // Compute bounds
        const int basis_x = xc;
        const int basis_y = yc + node_idx * 30;
        const int rect_x = basis_x + extents.x_bearing - padding;
        const int rect_y = basis_y + extents.y_bearing - padding;
        const int rect_w = extents.width + 2 * padding;
        const int rect_h = extents.height + 2 * padding;

        // Draw the rectangle body
        ctx->set_source_rgb(0.95, 0.95, 0.95);
        ctx->rectangle(rect_x, rect_y, rect_w, rect_h);
        ctx->fill();

        // Draw the rectangle border
        ctx->set_source_rgb(0.2, 0.2, 0.2);
        ctx->rectangle(rect_x, rect_y, rect_w, rect_h);
        ctx->stroke();

        // Draw text
        ctx->set_source_rgb(0.0, 0.0, 0.0);
        ctx->move_to(basis_x, basis_y);
        ctx->show_text(resolvent.formula);
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
