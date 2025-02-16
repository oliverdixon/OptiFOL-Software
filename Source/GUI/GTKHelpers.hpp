/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef GTKHELPERS_HPP
#define GTKHELPERS_HPP

#include <gtkmm.h>

namespace optifol
{
class GTKHelpers
{
public:
    template<class ObjectType>
    static Glib::RefPtr<ObjectType> get_object(const std::string &segment_name,
                                               const Glib::RefPtr<Gtk::Builder> builder,
                                               const std::string &object_name)
    {
        if (!builder)
            throw std::runtime_error("Could not build \"" + segment_name + "\": the builder was not initialised.");

        const auto object = builder->get_object<ObjectType>(object_name);

        if (!object)
            throw std::runtime_error("Could not build \"" + segment_name + "\": GTK object \"" + object_name + "\" "
                                     "was not found");

        return object;
    }

    template<class ObjectType>
    static ObjectType *get_widget(const std::string &segment_name,
                                  const Glib::RefPtr<Gtk::Builder> builder,
                                  const std::string &widget_name)
    {
        if (!builder)
            throw std::runtime_error("Could not build \"" + segment_name + "\": the builder was not initialised.");

        const auto widget = builder->get_widget<ObjectType>(widget_name);

        if (!widget)
            throw std::runtime_error("Could not build \"" + segment_name + "\": GTK widget \"" + widget_name + "\" "
                                     "was not found");

        return widget;
    }
};
}

#endif
