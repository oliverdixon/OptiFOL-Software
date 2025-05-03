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

#include "../Storage/StorageObjectBase.hpp"

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

    /**
     * @brief Set up a non-expandable GTK label within the given container
     * @param list_item The container into which the label should be emplaced
     * @param mono_styling Should the label be styled according to the standard monospace style?
     */
    static void on_setup_flat_label(const Glib::RefPtr<Gtk::ListItem> &list_item, const bool mono_styling = false)
    {
        const auto label = Gtk::make_managed<Gtk::Label>();

        label->set_halign(Gtk::Align::START);
        if (mono_styling)
            label->add_css_class("optifol_monospace");

        list_item->set_child(*label);
    }

    /**
     * @brief Set up an expandable GTK label within the given container
     * @param list_item The container into which the label should be emplaced
     * @param mono_styling Should the label be styled according to the standard monospace style?
     */
    static void on_setup_expandable_label(const Glib::RefPtr<Gtk::ListItem> &list_item, const bool mono_styling = false)
    {
        const auto expander = Gtk::make_managed<Gtk::TreeExpander>();
        const auto label = Gtk::make_managed<Gtk::Label>();

        label->set_halign(Gtk::Align::START);
        if (mono_styling)
            label->add_css_class("optifol_monospace");

        expander->set_child(*label);
        list_item->set_child(*expander);
    }

    /**
     * @brief Establish a property-synched binding between the 'name' property of a StorableObjectBase-like object, and
     *  a flat (non-expandable) label in a Gtk::ListView.
     * @param list_item The list item provided by the GTK callback invocation
     */
    static void on_bind_flat_name(const Glib::RefPtr<Gtk::ListItem> &list_item)
    {
        const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        const auto item = std::dynamic_pointer_cast<StorageObjectBase>(list_item->get_item());

        if (label != nullptr && item != nullptr)
            Glib::Binding::bind_property(item->property_name(), label->property_label(),
                Glib::Binding::Flags::SYNC_CREATE);
    }

    /**
     * @brief Establish a property-synched binding between the 'creation time' property of a StorableObjectBase-like
     *  object, and a flat (non-expandable) label in a Gtk::ListView by means of a locale-dependent formatting routine.
     * @param list_item The list item provided by the GTK callback invocation
     */
    static void on_bind_flat_creation_time(const Glib::RefPtr<Gtk::ListItem> &list_item)
    {
        const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        const auto item = std::dynamic_pointer_cast<StorageObjectBase>(list_item->get_item());

        if (label != nullptr && item != nullptr)
            Glib::Binding::bind_property(item->property_creation_time(), label->property_label(),
                Glib::Binding::Flags::SYNC_CREATE, [](const std::chrono::system_clock::time_point& time)
                {
                    return std::format("{:%c}", time);
                });
    }

    /**
     * @brief Establish a property-synched binding between the 'last-modified time' property of a StorableObjectBase-
     *  like object, and a flat (non-expandable) label in a Gtk::ListView by means of a locale-dependent formatting
     *  routine.
     * @param list_item The list item provided by the GTK callback invocation
     */
    static void on_bind_flat_modified_time(const Glib::RefPtr<Gtk::ListItem> &list_item)
    {
        const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
        const auto item = std::dynamic_pointer_cast<StorageObjectBase>(list_item->get_item());

        if (label != nullptr && item != nullptr)
            Glib::Binding::bind_property(item->property_modified_time(), label->property_label(),
                Glib::Binding::Flags::SYNC_CREATE, [](const std::chrono::system_clock::time_point& time)
                {
                    return std::format("{:%c}", time);
                });
    }

    /**
     * @brief Establish a property-synched binding between the 'name' property of a StorableObjectBase-like object, and
     *  a tree-expandable label in a Gtk::ListView with nested expanders.
     * @param list_item The list item provided by the GTK callback invocation
     * @param tree_model The tree model in which the list item exists, required to update expander responsibility
     *  delegation
     */
    static void on_bind_expandable_name(const Glib::RefPtr<Gtk::ListItem> &list_item,
        const Glib::RefPtr<Gtk::TreeListModel>& tree_model)
    {
        const auto position = list_item->get_position();
        const auto model_item = std::dynamic_pointer_cast<StorageObjectBase>(list_item->
            get_item());
        const auto expander = dynamic_cast<Gtk::TreeExpander*>(list_item->get_child());

        if (position == GTK_INVALID_LIST_POSITION || model_item == nullptr || expander == nullptr)
            return;

        const auto gui_row = tree_model->get_row(position);
        if (!gui_row)
            return;

        expander->set_list_row(gui_row);

        const auto label = dynamic_cast<Gtk::Label*>(expander->get_child());
        if (!label)
            return;

        Glib::Binding::bind_property(model_item->property_name(), label->property_label(),
            Glib::Binding::Flags::SYNC_CREATE);
    }
};

}

#endif
