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

const char * const RequirementsIndexArea::area_name = "Requirements Index Area";

RequirementsIndexArea::RequirementsIndexArea(Gtk::Builder& builder) :
    on_off_widgets(
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "requirements_index_advice_unselected"),
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "requirements_index_content")
    ),
    empty_widget(GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "requirements_index_advice_empty")),
    view(GTKHelpers::get_widget<Gtk::ColumnView>(area_name, builder, "requirements_view")),
    context_menu(
        view,
        GTKHelpers::get_object<Gio::Menu>(area_name, builder, "requirement_context_menu"),
        {
            {
                "new_requirement",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "new_requirement"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_requirement_popover"),
                true
            },
            {
                "edit_requirement",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "edit_requirement"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "edit_requirement_popover"),
                false
            },
            {
                "delete_requirement",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "delete_requirement"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "delete_requirement_popover"),
                false
            }
        }
    )
{
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    selection_model->property_n_items().signal_changed().connect([this]
    {
        empty_widget->set_visible(selection_model->get_n_items() == 0);
    });

    selection_model->signal_selection_changed().connect([this](guint, const guint n_items)
    {
        if (n_items == 0) {
            context_menu.disable_action("edit_requirement");
            context_menu.disable_action("delete_requirement");
            context_menu.disable_action("duplicate_requirement");
        } else {
            context_menu.enable_action("edit_requirement");
            context_menu.enable_action("delete_requirement");
            context_menu.enable_action("duplicate_requirement");
        }
    });

    selection_model->signal_items_changed().connect([this](guint, const guint removed, guint)
    {
        if (removed > 0) {
            // If anything was removed from the model, just deselect everything out of an abundance of caution.
            context_menu.disable_action("edit_requirement");
            context_menu.disable_action("delete_requirement");
            context_menu.disable_action("duplicate_requirement");
        }
    });

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
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                {
                    list_item->set_child(*Gtk::make_managed<Gtk::Label>());
                });

                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                {
                    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
                    const auto item = std::dynamic_pointer_cast<GRequirement>(list_item->get_item());

                    if (label != nullptr && item != nullptr)
                        Glib::Binding::bind_property(item->property_name(), label->property_label(),
                            Glib::Binding::Flags::SYNC_CREATE);
                });
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

    configure_new_requirement_popup(builder);
    configure_delete_requirement_popup(builder);
    configure_edit_requirement_popup(builder);
}

void RequirementsIndexArea::select_model(const Glib::RefPtr<Gio::ListStore<GRequirement>> &new_model)
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);
    empty_widget->set_visible(new_model->get_n_items() == 0);

    data_model = new_model;
    selection_model->set_model(new_model);
}

void RequirementsIndexArea::deselect_model() const
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);
    empty_widget->set_visible(false);

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

void RequirementsIndexArea::configure_new_requirement_popup(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_requirement_popover");
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_requirement_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_requirement_cancel");
    const auto property_name = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder,
        "new_requirement_property_name");
    const auto property_description = GTKHelpers::get_widget<Gtk::TextView>(area_name,
        builder, "new_requirement_property_description");
    const auto property_sentence = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder,
        "new_requirement_property_sentence");
    const auto property_test = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder,
        "new_requirement_property_test");
    const auto property_priority = GTKHelpers::get_widget<Gtk::DropDown>(area_name, builder,
        "new_requirement_property_priority");

    cancel_button->signal_clicked().connect([popover]
    {
        popover->popdown();
    });

    confirm_button->signal_clicked().connect([this, popover, property_name, property_description, property_sentence,
        property_test, property_priority]
    {
        popover->popdown();

        std::optional<std::size_t> test_id;
        if (property_test->get_text_length() > 0) {
            std::stringstream stream(property_test->get_text());
            std::size_t candidate;
            stream >> candidate;
            test_id.emplace(candidate);
        }

        const auto requirement = Glib::make_refptr_for_instance(new GRequirement());
        requirement->property_name().set_value(property_name->get_text());
        data_model->append(requirement);
    });
}

void RequirementsIndexArea::configure_edit_requirement_popup(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "edit_requirement_popover");
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "edit_requirement_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "edit_requirement_cancel");
    const auto property_name = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder,
        "edit_requirement_property_name");
    const auto property_description = GTKHelpers::get_widget<Gtk::TextView>(area_name,
        builder, "edit_requirement_property_description");
    const auto property_sentence = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder,
        "edit_requirement_property_sentence");
    const auto property_test = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder,
        "edit_requirement_property_test");
    const auto property_priority = GTKHelpers::get_widget<Gtk::DropDown>(area_name, builder,
        "edit_requirement_property_priority");

    cancel_button->signal_clicked().connect([popover]
    {
        popover->popdown();
    });

    popover->signal_show().connect([this, property_name, property_description, property_sentence, property_test,
        property_priority]
    {
        const auto candidate = std::dynamic_pointer_cast<GRequirement>(
            selection_model->get_selected_item());

        if (candidate != nullptr)
            property_name->set_text(candidate->property_name().get_value());
    });

    confirm_button->signal_clicked().connect([this, popover, property_name, property_description, property_sentence,
        property_test, property_priority]
    {
        popover->popdown();

        std::optional<std::size_t> test_id;
        if (property_test->get_text_length() > 0) {
            std::stringstream stream(property_test->get_text());
            std::size_t candidate;
            stream >> candidate;
            test_id.emplace(candidate);
        }

        const auto candidate = std::dynamic_pointer_cast<GRequirement>(
            selection_model->get_selected_item());
        if (candidate != nullptr)
            candidate->property_name().set_value(property_name->get_text());
    });
}

void RequirementsIndexArea::configure_delete_requirement_popup(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "delete_requirement_popover");
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder,
        "delete_requirement_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder,
        "delete_requirement_cancel");
    const auto property_name = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder,
        "delete_requirement_property_name");

    cancel_button->signal_clicked().connect([popover]
    {
        popover->popdown();
    });

    popover->signal_show().connect([this, property_name]
    {
#if 0
        const auto candidate = std::dynamic_pointer_cast<const Requirement>(
            selection_model->get_selected_item());
        if (candidate != nullptr)
            property_name->set_text(candidate->get_identifier());
#endif
    });

    confirm_button->signal_clicked().connect([this, popover]
    {
        popover->popdown();
        data_model->remove(selection_model->get_selected());
    });
}

std::pair<Glib::RefPtr<GRequirement>, Gtk::EditableLabel*> RequirementsIndexArea::on_bind_setup(
        const Glib::RefPtr<Gtk::ListItem> &list_item) const
{
#if 0
    const auto position = list_item->get_position();

    if (position == GTK_INVALID_LIST_POSITION)
        return {nullptr, nullptr}; // TODO log

    const auto model_item = data_model->get_typed_object<Requirement>(position);
    if (!model_item)
        return {nullptr, nullptr}; // TODO log

    const auto label = dynamic_cast<Gtk::EditableLabel*>(list_item->get_child());
    if (!label)
        return {model_item, nullptr}; // TODO log

    return {model_item, label};
#endif

    return {};
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

#if 0
    const auto model_item = data_model->get_typed_object<Requirement>(position);
    if (!model_item)
        return; // TODO log

    std::invoke(std::forward<SetterFunc>(setter_function), model_item, label->get_text());
#endif
}

}
