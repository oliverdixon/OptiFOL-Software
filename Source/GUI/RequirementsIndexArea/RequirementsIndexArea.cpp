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
#include "../../Storage/Subsystem.hpp"
#include "../GTKHelpers.hpp"

namespace optifol
{

const char * const RequirementsIndexArea::area_name = "Requirements Index Area";

RequirementsIndexArea::RequirementsIndexArea(Gtk::Builder& builder) :
    on_off_widgets(
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "requirements_index_advice_unselected"),
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "requirements_index_content")
    ),
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
            },
            {
                "duplicate_requirement",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "duplicate_requirement"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "duplicate_requirement_popover"),
                false
            }
        }
    ),
    new_requirement_popover(builder, *this),
    edit_requirement_popover(builder, *this),
    duplicate_requirement_popover(builder, *this),
    delete_requirement_popover(builder, *this)
{
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);

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
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&StorageObjectBase::bind_name));
            } else if (gtk_id == "requirement_statement") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, true));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_statement));
            } else if (gtk_id == "requirement_description") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_description));
            } else if (gtk_id == "requirement_priority") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_priority));
            } else if (gtk_id == "requirement_test") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_test_input));
            } else if (gtk_id == "requirement_created") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&StorageObjectBase::bind_creation_time));
            } else if (gtk_id == "requirement_modified") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&StorageObjectBase::bind_modification_time));
            } else
                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
            ++processed_columns;
        }
    }

#if 0 // TODO remaining fields
    if (processed_columns < column_count)
        assert(0);
#endif
}

void RequirementsIndexArea::select_model(const Glib::RefPtr<Subsystem> &new_subsystem)
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);

    active_subsystem = new_subsystem;
    new_subsystem->populate_selection_model(*selection_model);
}

void RequirementsIndexArea::deselect_model()
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);

    active_subsystem = nullptr;
    selection_model->set_model(nullptr);
}

Subsystem *RequirementsIndexArea::observe_active_subsystem() noexcept
{
    return active_subsystem.get();
}

const Subsystem *RequirementsIndexArea::observe_active_subsystem() const noexcept
{
    return active_subsystem.get();
}

Glib::RefPtr<Requirement> RequirementsIndexArea::get_selection() const
{
    const auto selected_item = selection_model->get_selected_item();

    if (selected_item == nullptr)
        throw std::runtime_error("No item selected in the selection model.");

    const auto selected_requirement = std::dynamic_pointer_cast<Requirement>(selected_item);

    if (selected_requirement == nullptr)
        throw std::runtime_error("Selected item is not a Requirement.");

    return selected_requirement;
}

void RequirementsIndexArea::on_bind_property_description(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    const auto item = std::dynamic_pointer_cast<Requirement>(list_item->get_item());

    if (label != nullptr && item != nullptr)
        Glib::Binding::bind_property(item->property_description(), label->property_label(),
            Glib::Binding::Flags::SYNC_CREATE);
}

void RequirementsIndexArea::on_bind_property_statement(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    const auto item = std::dynamic_pointer_cast<Requirement>(list_item->get_item());

    if (label != nullptr && item != nullptr)
        Glib::Binding::bind_property(item->property_statement(), label->property_label(),
            Glib::Binding::Flags::SYNC_CREATE,
            [item](const Glib::ustring&)
            {
                return item->get_formatted_statement();
            });
}

void RequirementsIndexArea::on_bind_property_priority(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    const auto item = std::dynamic_pointer_cast<Requirement>(list_item->get_item());

    if (label != nullptr && item != nullptr)
        Glib::Binding::bind_property(
                item->property_priority(), label->property_label(), Glib::Binding::Flags::SYNC_CREATE);
}

void RequirementsIndexArea::on_bind_property_test_input(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    const auto item = std::dynamic_pointer_cast<Requirement>(list_item->get_item());

    if (label != nullptr && item != nullptr)
        Glib::Binding::bind_property(
                item->property_test_input(), label->property_label(), Glib::Binding::Flags::SYNC_CREATE);
}

void RequirementsIndexArea::on_bind_property_normalised(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    const auto item = std::dynamic_pointer_cast<Requirement>(list_item->get_item());

    if (label != nullptr && item != nullptr)
        Glib::Binding::bind_property(
                item->property_normalised(), label->property_label(), Glib::Binding::Flags::SYNC_CREATE);
}

void RequirementsIndexArea::update_with_selected_name(Gtk::Entry &target) const
{
    const auto candidate = std::dynamic_pointer_cast<const Requirement>(selection_model->get_selected_item());
    if (candidate != nullptr)
        target.set_text(candidate->property_name().get_value());
}

} // namespace optifol
