/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for some handy exception-aware GTK static helpers
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#ifndef GTKHELPERS_HPP
#define GTKHELPERS_HPP

#include <gtkmm.h>

namespace optifol
{

/**
 * @namespace mp_helpers
 * @brief Metaprogramming UI helpers that are required to be declared at the namespace level
 */
namespace mp_helpers
{

/**
 * @class is_optional
 * @brief False case for testing specialisations of std::optional
 */
template<typename>
struct is_optional : std::false_type {};

/**
 * @class is_optional
 * @brief True case for testing specialisations of std::optional
 * @tparam T The type to test
 */
template<typename T>
struct is_optional<std::optional<T>> : std::true_type {};

/**
 * @class returns_optional
 * @brief False case for testing return-type optionality of const non-static member functions
 */
template<typename>
struct returns_optional : std::false_type {};

/**
 * @class returns_optional
 * @brief True case for testing return-type optionality of const non-static member functions
 * @tparam CT The class in which the candidate function is a constant member
 * @tparam RT The return type of the candidate function
 * @tparam Args The argument-type vector of the candidate function
 */
template<typename CT, typename RT, typename... Args>
struct returns_optional<RT (CT::*)(Args...) const> : is_optional<RT> {};

// Current Clang 18 bug reports Doxygen violations for uses of @tparam on templated concepts.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"

/**
 * @concept OptionalReturner
 * @brief Represents a function signature type, which represents that of a constant non-static member function returning
 *  some specialisation of std::optional.
 * @tparam FS The function signature of the function to test
 */
template<typename FS>
concept OptionalReturner = returns_optional<std::decay_t<FS>>::value;

/**
 * @concept GTKWidget
 * @brief Represents a concrete GTK widget
 * @tparam WidgetType The concrete widget type
 */
template<typename WidgetType>
concept GTKWidget = std::derived_from<WidgetType, Gtk::Widget> && !std::is_abstract_v<WidgetType>;

/**
 * @concept GTKObject
 * @brief Represents a concrete GTK object
 * @tparam ObjectType The concrete object type
 */
template<typename ObjectType>
concept GTKObject = std::derived_from<ObjectType, Glib::Object> && !std::is_abstract_v<ObjectType>;

#pragma clang diagnostic pop

}

/**
 * @class GTKHelpers
 * @brief A handy set of modern exception-aware (throwing) wrappers for common GTKmm operations
 */
class GTKHelpers
{
public:
    /**
     * @brief Retrieve a GTK object as a Glib-wrapped ref-counted pointer from a GTK builder
     * @tparam ObjectType The expected concrete GTK object return type from the builder
     * @param segment_name The name of the context requesting the item; used for enhanced error logging.
     * @param builder The builder with an expected reference to the item
     * @param object_name The ID of the target GTK object
     * @return A Glib-wrapped instance of the GTK object typed according to the template parameter
     * @throws std::runtime_error The named object could not be built according to the given type and name
     */
    template<mp_helpers::GTKObject ObjectType>
    static Glib::RefPtr<ObjectType> get_object(const std::string &segment_name, Gtk::Builder& builder,
        const std::string &object_name)
    {
        const auto object = builder.get_object<ObjectType>(object_name);
        if (!object)
            throw std::runtime_error("Could not build \"" + segment_name + "\": GTK object \"" + object_name + "\" "
                "was not found");
        return object;
    }

    /**
     * @brief Retrieve a GTK widget as a raw pointer (ultimately managed by the GTK object system) from a GTK builder
     * @tparam WidgetType The expected concrete GTK widget return type from the builder
     * @param segment_name The name of the context requesting the item; used for enhanced error logging.
     * @param builder The builder with an expected reference to the item
     * @param widget_name The ID of the target GTK widget
     * @return A Glib-wrapped instance of the GTK widget typed according to the template parameter
     * @throws std::runtime_error The named widget could not be built according to the given type and name
     */
    template<mp_helpers::GTKWidget WidgetType>
    static WidgetType *get_widget(const std::string &segment_name,
                                  Gtk::Builder& builder,
                                  const std::string &widget_name)
    {
        const auto widget = builder.get_widget<WidgetType>(widget_name);
        if (!widget)
            throw std::runtime_error("Could not build \"" + segment_name + "\": GTK widget \"" + widget_name + "\" "
                "was not found");

        return widget;
    }
};

}

#endif
