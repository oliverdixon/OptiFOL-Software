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
    )
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
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&GTKHelpers::on_bind_flat_name));
            } else if (gtk_id == "requirement_statement") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, true));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_statement));
            } else if (gtk_id == "requirement_description") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_description));
            } else if (gtk_id == "requirement_priority") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_priority));
            } else if (gtk_id == "requirement_created") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&GTKHelpers::on_bind_flat_creation_time));
            } else if (gtk_id == "requirement_modified") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&GTKHelpers::on_bind_flat_modified_time));
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

    configure_new_requirement_popover(builder);
    configure_delete_requirement_popover(builder);
    configure_edit_requirement_popover(builder);
    configure_duplicate_requirement_popover(builder);
}

void RequirementsIndexArea::select_model(const Glib::RefPtr<const Subsystem> &subsystem_model)
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);

    data_model = subsystem_model->requirements;
    selection_model->set_model(data_model);
}

void RequirementsIndexArea::deselect_model()
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);

    data_model = nullptr;
    selection_model->set_model(nullptr);
}

void RequirementsIndexArea::configure_new_requirement_popover(Gtk::Builder &builder)
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_requirement_popover");
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_requirement_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_requirement_cancel");
    const auto property_name = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "new_requirement_property_name");
    const auto property_description = GTKHelpers::get_widget<Gtk::TextView>(area_name, builder, "new_requirement_property_description");
    const auto property_sentence = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "new_requirement_property_sentence");
    const auto property_test = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "new_requirement_property_test");
    const auto property_priority = GTKHelpers::get_widget<Gtk::DropDown>(area_name, builder, "new_requirement_property_priority");

    cancel_button->signal_clicked().connect([popover]
    {
        popover->popdown();
    });

    confirm_button->signal_clicked().connect([this, popover, property_name, property_description, property_sentence,
        property_test, property_priority]
    {
        popover->popdown();

        std::optional<std::size_t> test_id; // TODO
        if (property_test->get_text_length() > 0) {
            std::stringstream stream(property_test->get_text());
            std::size_t candidate;
            stream >> candidate;
            test_id.emplace(candidate);
        }

        data_model->append(Glib::make_refptr_for_instance(new Requirement(
            property_name->get_text(),
            property_sentence->get_text(),
            property_description->get_buffer()->get_text(),
            property_priority->get_selected(),
            symbol_repository
        )));
    });
}

void RequirementsIndexArea::configure_edit_requirement_popover(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "edit_requirement_popover");
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "edit_requirement_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "edit_requirement_cancel");
    const auto property_name = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "edit_requirement_property_name");
    const auto property_description = GTKHelpers::get_widget<Gtk::TextView>(area_name, builder, "edit_requirement_property_description");
    const auto property_sentence = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "edit_requirement_property_sentence");
    const auto property_test = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "edit_requirement_property_test");
    const auto property_priority = GTKHelpers::get_widget<Gtk::DropDown>(area_name, builder, "edit_requirement_property_priority");

    cancel_button->signal_clicked().connect([popover]
    {
        popover->popdown();
    });

    popover->signal_show().connect([this, property_name, property_description, property_sentence, property_test,
        property_priority]
    {
        const auto candidate = std::dynamic_pointer_cast<Requirement>(
            selection_model->get_selected_item());

        if (candidate != nullptr) {
            property_name->set_text(candidate->property_name().get_value());
            property_description->get_buffer()->set_text(candidate->property_description().get_value());
            property_sentence->set_text(candidate->property_statement().get_value());
            property_priority->set_selected(candidate->property_priority().get_value());
        }
    });

    confirm_button->signal_clicked().connect([this, popover, property_name, property_description, property_sentence,
        property_test, property_priority]
    {
        popover->popdown();

        std::optional<std::size_t> test_id; // TODO
        if (property_test->get_text_length() > 0) {
            std::stringstream stream(property_test->get_text());
            std::size_t candidate;
            stream >> candidate;
            test_id.emplace(candidate);
        }

        const auto candidate = std::dynamic_pointer_cast<Requirement>(
            selection_model->get_selected_item());

        if (candidate != nullptr) {
            candidate->property_name().set_value(property_name->get_text());
            candidate->property_description().set_value(property_description->get_buffer()->get_text());
            candidate->property_statement().set_value(property_sentence->get_text());
            candidate->property_priority().set_value(property_priority->get_selected());
        }
    });
}

void RequirementsIndexArea::configure_delete_requirement_popover(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "delete_requirement_popover");
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "delete_requirement_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "delete_requirement_cancel");
    const auto property_name = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "delete_requirement_property_name");

    cancel_button->signal_clicked().connect([popover]
    {
        popover->popdown();
    });

    popover->signal_show().connect([this, property_name]
    {
        const auto candidate = std::dynamic_pointer_cast<const Requirement>(
            selection_model->get_selected_item());
        if (candidate != nullptr)
            property_name->set_text(candidate->property_name().get_value());
    });

    confirm_button->signal_clicked().connect([this, popover]
    {
        popover->popdown();
        data_model->remove(selection_model->get_selected());
    });
}

void RequirementsIndexArea::configure_duplicate_requirement_popover(Gtk::Builder &builder)
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "duplicate_requirement_popover");
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "duplicate_requirement_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "duplicate_requirement_cancel");
    const auto property_old_name = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "duplicate_requirement_property_old_name");
    const auto property_new_name = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "duplicate_requirement_property_new_name");

    cancel_button->signal_clicked().connect([popover]
    {
        popover->popdown();
    });

    popover->signal_show().connect([this, property_old_name]
    {
        const auto candidate = std::dynamic_pointer_cast<const Requirement>(
            selection_model->get_selected_item());
        if (candidate != nullptr)
            property_old_name->set_text(candidate->property_name().get_value());
    });

    confirm_button->signal_clicked().connect([this, popover, property_new_name]
    {
        popover->popdown();

        const auto candidate = std::dynamic_pointer_cast<const Requirement>(
            selection_model->get_selected_item());

        if (candidate != nullptr)
            data_model->append(Glib::make_refptr_for_instance(new Requirement(
                property_new_name->get_text(),
                candidate->property_statement().get_value(),
                candidate->property_description().get_value(),
                candidate->property_priority().get_value(),
                symbol_repository
            )));
    });
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
        Glib::Binding::bind_property(item->property_priority(), label->property_label(),
            Glib::Binding::Flags::SYNC_CREATE);
}

void RequirementsIndexArea::on_bind_property_normalised(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    const auto item = std::dynamic_pointer_cast<Requirement>(list_item->get_item());

    if (label != nullptr && item != nullptr)
        Glib::Binding::bind_property(item->property_normalised(), label->property_label(),
            Glib::Binding::Flags::SYNC_CREATE);
}

}
