/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef REPORTSAREA_HPP
#define REPORTSAREA_HPP

#include <gtkmm/builder.h>

namespace optifol
{

class ReportsArea :
        public sigc::trackable
{
public:
    explicit ReportsArea(Gtk::Builder& builder);
};

} // namespace optifol

#endif // REPORTSAREA_HPP
