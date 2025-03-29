/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Main Window's Requirements Index UI area
 * @author Oliver Dixon
 * @date 2025-03-29
 * @version Development
 */

#include "RequirementsIndexArea.hpp"

#include <iostream>

namespace optifol
{

RequirementsIndexArea::RequirementsIndexArea(Gtk::ColumnView *view,
        const Glib::RefPtr<PGRequirementModel> &initial_model) :
    selection_model(Gtk::SingleSelection::create(initial_model))
{
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    view->set_model(selection_model);

    const auto columns = view->get_columns();
    const auto column_count = columns->get_n_items();
    guint processed_columns = 0;

    for (guint position = 0; position < column_count; ++position) {
        Glib::RefPtr<Gtk::ColumnViewColumn> column = nullptr;

        if ((column = columns->get_typed_object<Gtk::ColumnViewColumn>(position)) != nullptr) {
            const auto& gtk_id = column->get_id();
            const auto factory = Gtk::SignalListItemFactory::create();

            if (gtk_id == "requirement_name") {
                factory->signal_setup().connect(sigc::ptr_fun(&RequirementsIndexArea::on_setup_static_mono));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_identifier); });
            } else if (gtk_id == "requirement_statement") {
                factory->signal_setup().connect(sigc::ptr_fun(&RequirementsIndexArea::on_setup_static_mono));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_statement); });
            } else if (gtk_id == "requirement_description") {
                factory->signal_setup().connect(sigc::ptr_fun(&RequirementsIndexArea::on_setup_static));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_description); });
            } else if (gtk_id == "requirement_priority") {
                factory->signal_setup().connect(sigc::ptr_fun(&RequirementsIndexArea::on_setup_static));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_priority); });
            } else if (gtk_id == "requirement_test") {
                factory->signal_setup().connect(sigc::ptr_fun(&RequirementsIndexArea::on_setup_static));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_test); });
            } else if (gtk_id == "requirement_stakeholder") {
                factory->signal_setup().connect(sigc::ptr_fun(&RequirementsIndexArea::on_setup_static));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_stakeholder); });
            } else
                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
            ++processed_columns;
        }
    }

#if 0
    if (processed_columns < column_count)
        assert(0); // TODO exception / log4cxx
#endif
}

void RequirementsIndexArea::set_model(const Glib::RefPtr<PGRequirementModel> &new_model) const
{
    selection_model->set_model(new_model);
}

void RequirementsIndexArea::on_setup_static(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = Gtk::make_managed<Gtk::Label>();
    label->set_halign(Gtk::Align::START);
    list_item->set_child(*label);
}

void RequirementsIndexArea::on_setup_static_mono(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = Gtk::make_managed<Gtk::Label>();
    label->set_halign(Gtk::Align::START);
    label->add_css_class("optifol_fol_statement");
    list_item->set_child(*label);
}

void RequirementsIndexArea::on_setup_edit(const Glib::RefPtr<Gtk::ListItem> &list_item,
                                          Glib::SignalProxyProperty::SlotType &&edit_callback)
{
}

std::pair<Glib::RefPtr<Requirement>, Gtk::Label*> RequirementsIndexArea::on_bind_setup(
        const Glib::RefPtr<Gtk::ListItem> &list_item) const
{
    const auto position = list_item->get_position();

    if (position == GTK_INVALID_LIST_POSITION)
        return {nullptr, nullptr}; // TODO log

    const auto model_item = selection_model->get_model()->get_typed_object<Requirement>(position);
    if (!model_item)
        return {nullptr, nullptr}; // TODO log

    const auto label = dynamic_cast<Gtk::Label*>(list_item->get_child());
    if (!label)
        return {model_item, nullptr}; // TODO log

    return {model_item, label};
}

template<typename InfoFunc>
void RequirementsIndexArea::on_bind_label(const Glib::RefPtr<Gtk::ListItem> &list_item,
        InfoFunc &&information_function) const
{
    const auto [model_item, label] = on_bind_setup(list_item);
    if (model_item == nullptr || label == nullptr)
        return;

    const auto& value = std::invoke(std::forward<InfoFunc>(information_function), model_item);

    if constexpr (std::is_convertible_v<Glib::ustring, decltype(value)>)
        // If the information function will give us something convertible to a GTK string, just pass it through.
        label->set_text(value);
    else
        /*
         * Otherwise, we're in the dark. Rely on ADR to see if std::to_string has an overload. This is typically useful
         * for standard-typed numbers.
         */
        label->set_text(std::to_string(value));
}

template<mp_helpers::OptionalReturner InfoFunc>
void RequirementsIndexArea::on_bind_label(const Glib::RefPtr<Gtk::ListItem> &list_item,
        InfoFunc &&information_function) const
{
    const auto [model_item, label] = on_bind_setup(list_item);
    if (model_item == nullptr || label == nullptr)
        return;

    /*
     * Performance note: GtkWidget::has_css_class is often computable in constant time. Worst-case linear in the number
     * of CSS classes applied to the node, which is typically small. GtkWidget::remove_css_class is not defined at an
     * API level to handle invalid requests gracefully (i.e., if a caller requested to remove a CSS class that was not
     * associated to the node). These performance-critical label-binding functions can't afford to be exception-aware.
     */
    static const auto unknown_css_class_name = "optifol_unknown";
    const bool is_already_unknown = label->has_css_class(unknown_css_class_name);
    bool unknown_value = false;

    const auto& optional_value = std::invoke(std::forward<InfoFunc>(information_function), model_item);
    if (optional_value.has_value()) {
        if constexpr (std::is_convertible_v<Glib::ustring, decltype(*optional_value)>)
            // If we have plain string value, just pass it through.
            label->set_text(*optional_value);
        else
            // Otherwise, rely on the standard conversion functions with ADR.
            label->set_text(std::to_string(*optional_value));
    } else {
        label->set_text("Unassociated");
        unknown_value = true;
    }

    if (is_already_unknown && !unknown_value)
        label->remove_css_class(unknown_css_class_name);
    else if (!is_already_unknown && unknown_value)
        label->add_css_class(unknown_css_class_name);

}

}
