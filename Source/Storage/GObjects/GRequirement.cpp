/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 4/26/25.
//

#include "GRequirement.hpp"

#include <iostream>

namespace optifol
{

GRequirement::GRequirement() :
    Glib::ObjectBase("GRequirement"),
    name(*this, "grequirement-name")
{
}

GRequirement::GRequirement(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refBuilder) :
    Glib::ObjectBase("GRequirement"),
    Glib::Object(cobject),
    name(*this, "grequirement-name")
{
    property_name().signal_changed().connect([]
    {
        std::cout << "Name changed!" << std::endl;
    });
}

Glib::PropertyProxy<Glib::ustring> GRequirement::property_name()
{
    return name.get_proxy();
}

}
