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

namespace optifol
{

RequirementsIndexArea::RequirementsIndexArea(Gtk::ColumnView *view, Gtk::Widget * widget_off, Gtk::Widget * widget_on) :
    on_off_widgets(widget_off, widget_on)
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
                factory->signal_setup().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { on_setup_label(list_item, &Requirement::set_identifier, true); });
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_identifier); });
            } else if (gtk_id == "requirement_statement") {
                factory->signal_setup().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { on_setup_label(list_item, &Requirement::attempt_set_statement, true); });
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_statement); });
            } else if (gtk_id == "requirement_description") {
                factory->signal_setup().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { on_setup_label(list_item, &Requirement::set_description); });
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_description); });
            } else if (gtk_id == "requirement_priority") {
                factory->signal_setup().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { on_setup_label(list_item, &Requirement::attempt_set_priority); });
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_priority); });
            } else if (gtk_id == "requirement_test") {
                factory->signal_setup().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { on_setup_label(list_item, &Requirement::attempt_set_test); });
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_label(list_item, &Requirement::get_test); });
            } else if (gtk_id == "requirement_stakeholder") {
                factory->signal_setup().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { on_setup_label(list_item, &Requirement::attempt_set_stakeholder); });
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

void RequirementsIndexArea::select_model(const Glib::RefPtr<SubsystemHierarchicalModel> &new_model) const
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);
    selection_model->set_model(new_model);
}

void RequirementsIndexArea::deselect_model() const
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);
    selection_model->set_model(nullptr);
}

template<typename SetterFunc>
void RequirementsIndexArea::on_setup_label(const Glib::RefPtr<Gtk::ListItem> &list_item, SetterFunc&& setter_function,
        const bool mono_styling)
{
    const auto label = Gtk::make_managed<Gtk::EditableLabel>();

    /*
     * Subtlety note: this setup function is called when a particular Gtk::ListItem is being constructed for usage in
     * the Gtk::ColumnView. Between being constructed and later edited, the list item shared pointer may go out of
     * scope. To avoid this, and express correct ownership semantics, the list item is captured by value by this lambda.
     * It is then passed to the label-edit callback by reference for short-term observation within the lifetime scope of
     * the lambda. Thus, the below lambda function itself holds ownership of the ListItem.
     *
     * Once the Gtk::EditableLabel to which the signal is attached is destructed, this lambda will go out of scope and
     * release ownership, allowing the list item reference to be destructed appropriately.
     */
    label->property_editing().signal_changed().connect([this, list_item, setter_function]
    {
        on_edit_label(list_item, setter_function);
    });

    label->set_halign(Gtk::Align::START);
    if (mono_styling)
        label->add_css_class("optifol_monospace");

    list_item->set_child(*label);
}

std::pair<Glib::RefPtr<Requirement>, Gtk::EditableLabel*> RequirementsIndexArea::on_bind_setup(
        const Glib::RefPtr<Gtk::ListItem> &list_item) const
{
    const auto position = list_item->get_position();

    if (position == GTK_INVALID_LIST_POSITION)
        return {nullptr, nullptr}; // TODO log

    const auto model_item = selection_model->get_model()->get_typed_object<Requirement>(position);
    if (!model_item)
        return {nullptr, nullptr}; // TODO log

    const auto label = dynamic_cast<Gtk::EditableLabel*>(list_item->get_child());
    if (!label)
        return {model_item, nullptr}; // TODO log

    return {model_item, label};
}

template<typename GetterFunc>
void RequirementsIndexArea::on_bind_label(const Glib::RefPtr<Gtk::ListItem> &list_item,
        GetterFunc &&getter_function) const
{
    const auto [model_item, label] = on_bind_setup(list_item);
    if (model_item == nullptr || label == nullptr)
        return;

    const auto& value = std::invoke(std::forward<GetterFunc>(getter_function), model_item);

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

template<mp_helpers::OptionalReturner GetterFunc>
void RequirementsIndexArea::on_bind_label(const Glib::RefPtr<Gtk::ListItem> &list_item,
        GetterFunc &&getter_function) const
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

    const auto& optional_value = std::invoke(std::forward<GetterFunc>(getter_function), model_item);
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

template<typename SetterFunc>
void RequirementsIndexArea::on_edit_label(const Glib::RefPtr<Gtk::ListItem> &list_item, SetterFunc&& setter_function)
{
    const auto position = list_item->get_position();

    if (position == GTK_INVALID_LIST_POSITION)
        return; // TODO log

    const auto label = dynamic_cast<Gtk::EditableLabel*>(list_item->get_child());
    if (!label)
        return; // TODO log

    if (label->get_editing())
        /*
         * The Gtk::EditableLabel signal is very trigger-happy, and often fires before changes have been fully committed
         * (e.g. while the user is still typing). Ensure that we only process changes that have been committed to the
         * entry field.
         */
        return;

    const auto model_item = selection_model->get_model()->get_typed_object<Requirement>(position);
    if (!model_item)
        return; // TODO log

    std::invoke(std::forward<SetterFunc>(setter_function), model_item, label->get_text());
}

}
