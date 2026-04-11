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

#include <assert.h>
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
 * @brief False case for testing specialisations of @ref std::optional
 */
template<typename>
struct is_optional : std::false_type
{
};

/**
 * @class is_optional
 * @brief True case for testing specialisations of @ref std::optional
 * @tparam T The type to test
 */
template<typename T>
struct is_optional<std::optional<T>> : std::true_type
{
};

// Current Clang 18 bug reports Doxygen violations for uses of @tparam on templated concepts.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"

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

} // namespace mp_helpers

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
     * @param segment_name A view of the name of the context requesting the item; used for enhanced error
     *  logging.
     * @param builder The builder with an expected reference to the item
     * @param object_name The ID of the target GTK object
     * @return A Glib-wrapped instance of the GTK object typed according to the template parameter
     * @throws std::runtime_error The named object could not be built according to the given type and name
     */
    template<mp_helpers::GTKObject ObjectType>
    static Glib::RefPtr<ObjectType> get_object(
            const std::string_view segment_name, Gtk::Builder &builder, const std::string &object_name)
    {
        const auto object = builder.get_object<ObjectType>(object_name);
        if (!object)
            throw std::runtime_error("Could not build \"" + std::string(segment_name) + "\": GTK object \"" +
                    object_name + "\" was not found");
        return object;
    }

    /**
     * @brief Retrieve a GTK widget as a raw pointer (ultimately managed by the GTK object system) from a GTK
     *  builder
     * @tparam WidgetType The expected concrete GTK widget return type from the builder
     * @param segment_name A view of the name of the context requesting the item; used for enhanced error
     *  logging.
     * @param builder The builder with an expected reference to the item
     * @param widget_name The ID of the target GTK widget
     * @return A Glib-wrapped instance of the GTK widget typed according to the template parameter
     * @throws std::runtime_error The named widget could not be built according to the given type and name
     */
    template<mp_helpers::GTKWidget WidgetType>
    static WidgetType *get_widget(
            const std::string_view segment_name, Gtk::Builder &builder, const std::string &widget_name)
    {
        const auto widget = builder.get_widget<WidgetType>(widget_name);
        if (!widget)
            throw std::runtime_error("Could not build \"" + std::string(segment_name) + "\": GTK widget \"" +
                    widget_name + "\" was not found");

        return widget;
    }

    /**
     * @brief Set up a non-expandable GTK label within the given container
     * @param list_item The container into which the label should be emplaced
     * @param mono_styling Should the label be styled according to the standard monospace style?
     */
    static void setup_label(const Glib::RefPtr<Gtk::ListItem> &list_item, const bool mono_styling = false)
    {
        const auto label = Gtk::make_managed<Gtk::Label>();

        label->set_halign(Gtk::Align::START);
        if (mono_styling)
            label->add_css_class("optifol_monospace");

        list_item->set_child(*label);
    }

    /**
     * @brief Set up an editable text field (Gtk::Entry) within the given container
     * @param list_item The container into which the field should be emplaced
     */
    static void setup_entry(const Glib::RefPtr<Gtk::ListItem> &list_item)
    {
        const auto entry = Gtk::make_managed<Gtk::Entry>();
        entry->set_halign(Gtk::Align::START);
        list_item->set_child(*entry);
    }

    /**
     * @brief Set up a combo box (Gtk::DropDown) within the given container
     * @param list_item The container into which the combo box should be emplaced
     */
    static void setup_combo_box(const Glib::RefPtr<Gtk::ListItem> &list_item)
    {
        const auto combo_box = Gtk::make_managed<Gtk::DropDown>();
        combo_box->set_halign(Gtk::Align::START);
        list_item->set_child(*combo_box);
    }

    /**
     * @brief Set up an expandable GTK label within the given container
     * @param list_item The container into which the label should be emplaced
     * @param mono_styling Should the label be styled according to the standard monospace style?
     */
    static void setup_expandable_label(
            const Glib::RefPtr<Gtk::ListItem> &list_item, const bool mono_styling = false)
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
     * @brief Bind an arbitrary optional Glib::Property to a Gtk::Label
     * @tparam BoundType The @ref std::optional specialisation type to be stringified and bound
     * @tparam StoredType The class providing the property getter
     * @param property_functor Unbound member function functor to provide observing property proxy, i.e.
     *  Glib::PropertyProxy_ReadOnly.
     * @param object The object instance containing the property to be bound
     * @param label The destination label to contain a string representation of the property
     * @details If the property functor provides a @ref std::optional containing a value, the populated label
     *  is equivalent to the one provided by the non-@ref std::optional @ref bind_any_property. If the
     *  supplied property does not contain a value, the label is marked "Empty" and styled with the
     *  @ref unknown_css_class_name CSS class.
     */
    template<typename BoundType, typename StoredType>
        requires mp_helpers::is_optional<BoundType>::value
    static void bind_any_property(
            sigc::mem_functor<Glib::PropertyProxy_ReadOnly<BoundType> (StoredType::*)() const>
                    property_functor,
            const StoredType &object, Gtk::Label *const label)
    {
        if (label == nullptr)
            return;

        Glib::Binding::bind_property(property_functor.operator()(object), label->property_label(),
                Glib::Binding::Flags::SYNC_CREATE,
                [label](const BoundType &from) -> std::optional<Glib::ustring>
                {
                    const bool is_already_unknown = label->has_css_class(unknown_css_class_name);
                    bool unknown_value = false;
                    std::string string_value;

                    if (from.has_value()) {
                        if constexpr (std::is_convertible_v<Glib::ustring, decltype(*from)>)
                            // If we have plain string value, just pass it through.
                            string_value = *from;
                        else
                            // Otherwise, rely on the standard conversion functions with ADR.
                            string_value = std::to_string(*from);
                    } else {
                        unknown_value = true;
                        string_value = "Empty";
                    }

                    if (is_already_unknown && !unknown_value)
                        label->remove_css_class(unknown_css_class_name);
                    else if (!is_already_unknown && unknown_value)
                        label->add_css_class(unknown_css_class_name);

                    return string_value;
                });
    }

    /**
     * @brief Bind an arbitrary non-optional Glib::Property to a Gtk::Label
     * @tparam BoundType The type to be stringified and bound
     * @tparam StoredType The class providing the property getter
     * @param property_functor Unbound member function functor to provide observing property proxy, i.e.
     *  Glib::PropertyProxy_ReadOnly.
     * @param object The object instance containing the property to be bound
     * @param label The destination label to contain a string representation of the property
     */
    template<typename BoundType, typename StoredType>
    static void bind_any_property(
            sigc::mem_functor<Glib::PropertyProxy_ReadOnly<BoundType> (StoredType::*)() const>
                    property_functor,
            const StoredType &object, Gtk::Label *const label)
    {
        if (label == nullptr)
            return;

        Glib::Binding::bind_property(property_functor.operator()(object), label->property_label(),
                Glib::Binding::Flags::SYNC_CREATE);
    }

private:
    static constexpr auto unknown_css_class_name = "optifol_unknown";
};

} // namespace optifol

#endif
