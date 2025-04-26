/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef GREQUIREMENT_HPP
#define GREQUIREMENT_HPP

#include <glibmm/property.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/object.h>

namespace optifol
{

class GRequirement :
        public Glib::Object
{
public:
    GRequirement();

    GRequirement(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);

    Glib::PropertyProxy<Glib::ustring> property_name();

private:
    Glib::Property<Glib::ustring> name;
};

}

#endif
