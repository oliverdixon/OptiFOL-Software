/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Project Hierarchical view in the Optifol Main Window
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#include "ProjectHierarchyPane.hpp"

#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "../Storage/Subsystem.hpp"

namespace optifol
{

std::shared_ptr<log4cxx::Logger> ProjectHierarchyPane::logger(log4cxx::Logger::getLogger("Optifol"));

const char *const ProjectHierarchyPane::area_name = "Project Pane Area";

ProjectHierarchyPane::ProjectHierarchyPane(
        Gtk::Builder &builder, const Glib::RefPtr<Gio::ListStore<Project>> &initial_model) :
    stack_switcher(GTKHelpers::get_widget<Gtk::DropDown>(area_name, builder, "project_pane_switcher")),
    stack(GTKHelpers::get_widget<Gtk::Stack>(area_name, builder, "project_pane_stack")),
    view(GTKHelpers::get_widget<Gtk::ListView>(area_name, builder, "project_view")),
    data_model(initial_model),
    tree_model(Gtk::TreeListModel::create(
            data_model, sigc::ptr_fun(&ProjectHierarchyPane::tree_node_expand), true, true)),
    context_menu(view, GTKHelpers::get_object<Gio::Menu>(area_name, builder, "structure_context_menu"),
            {{"new_project", GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "new_project"),
                     GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_project_popover"), true},
                    {"new_subsystem",
                            GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "new_subsystem"),
                            GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_subsystem_popover"),
                            false},
                    {"edit_structure",
                            GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "edit_structure"),
                            GTKHelpers::get_widget<Gtk::Popover>(
                                    area_name, builder, "edit_structure_popover"),
                            false},
                    {"delete_structure",
                            GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "delete_structure"),
                            GTKHelpers::get_widget<Gtk::Popover>(
                                    area_name, builder, "delete_structure_popover"),
                            false}})
{
    view->signal_activate().connect(sigc::mem_fun(*this, &ProjectHierarchyPane::switch_selection));

    stack_switcher->property_selected().signal_changed().connect(
            sigc::mem_fun(*this, &ProjectHierarchyPane::switch_visible_stack));

    selection_model = Gtk::SingleSelection::create(tree_model);
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    view->set_model(selection_model);

    selection_model->signal_selection_changed().connect(
            [this](guint, const guint n_items)
            {
                if (n_items == 0) {
                    emit_deselected();
                    context_menu.disable_action("new_subsystem");
                    context_menu.disable_action("edit_structure");
                    context_menu.disable_action("delete_structure");
                } else {
                    context_menu.enable_action("new_subsystem");
                    context_menu.enable_action("edit_structure");
                    context_menu.enable_action("delete_structure");
                }
            });

    selection_model->signal_items_changed().connect(
            [this](guint, const guint removed, guint)
            {
                if (removed > 0) {
                    // If anything was removed from the model, just deselect everything out of an abundance of
                    // caution.
                    emit_deselected();
                    context_menu.disable_action("new_subsystem");
                    context_menu.disable_action("edit_structure");
                    context_menu.disable_action("delete_structure");
                }
            });

    const auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_expandable_label, false));
    factory->signal_bind().connect(sigc::bind(&StorageObjectBase::bind_name_property_expandable, tree_model));
    view->set_factory(factory);

    configure_new_project_popover(builder);
    configure_new_subsystem_popover(builder);
    configure_edit_structure_popover(builder);
    configure_delete_structure_popover(builder);
}

void ProjectHierarchyPane::add_subsystem_change_callback(
        sigc::slot<void(const Glib::RefPtr<Subsystem> &)> &&selected, sigc::slot<void()> &&deselected,
        const bool onboard)
{
    auto &callback = subsystem_change_callbacks.emplace_back();
    callback.first.connect(std::move(selected));
    callback.second.connect(std::move(deselected));

    if (onboard) {
        const auto subsystem = std::dynamic_pointer_cast<Subsystem>(selection_model->get_selected_item());
        if (subsystem != nullptr)
            callback.first(subsystem);
        else
            callback.second();
    }
}

void ProjectHierarchyPane::configure_new_project_popover(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_project_popover");
    const auto confirm_button =
            GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_project_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_project_cancel");
    const auto property_name =
            GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "new_project_property_name");

    cancel_button->signal_clicked().connect(
            [popover, property_name]
            {
                popover->popdown();
                property_name->set_text("");
            });

    confirm_button->signal_clicked().connect(
            [this, popover, property_name]
            {
                popover->popdown();
                data_model->append(Glib::make_refptr_for_instance(new Project(property_name->get_text())));
                property_name->set_text("");
            });
}

void ProjectHierarchyPane::configure_new_subsystem_popover(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_subsystem_popover");
    const auto confirm_button =
            GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_subsystem_confirm");
    const auto cancel_button =
            GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_subsystem_cancel");
    const auto property_path =
            GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "new_subsystem_property_path");
    const auto property_name =
            GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "new_subsystem_property_name");

    popover->signal_show().connect(
            [this, property_path]
            {
                const auto candidate =
                        std::dynamic_pointer_cast<const TreeNode>(selection_model->get_selected_item());

                if (candidate != nullptr)
                    property_path->set_text(candidate->get_path());
            });

    cancel_button->signal_clicked().connect(
            [popover, property_name]
            {
                popover->popdown();
                property_name->set_text("");
            });

    confirm_button->signal_clicked().connect(
            [this, popover, property_name]
            {
                popover->popdown();

                const auto candidate =
                        std::dynamic_pointer_cast<TreeNode>(selection_model->get_selected_item());
                if (candidate != nullptr)
                    candidate->add(property_name->get_text());

                property_name->set_text("");
            });
}

void ProjectHierarchyPane::configure_edit_structure_popover(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "edit_structure_popover");
    const auto confirm_button =
            GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "edit_structure_confirm");
    const auto cancel_button =
            GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "edit_structure_cancel");
    const auto property_old_path =
            GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "edit_structure_property_old_path");
    const auto property_new_path =
            GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "edit_structure_property_new_path");

    popover->signal_show().connect(
            [this, property_old_path, property_new_path]
            {
                const auto candidate =
                        std::dynamic_pointer_cast<const TreeNode>(selection_model->get_selected_item());

                if (candidate != nullptr) {
                    const auto current_path = candidate->get_path();
                    property_old_path->set_text(current_path);

                    const auto name_delim_position = current_path.rfind('/');
                    property_new_path->set_text(name_delim_position == std::string::npos
                                    ? current_path
                                    : current_path.substr(0, name_delim_position + 1));
                }
            });

    cancel_button->signal_clicked().connect([popover] { popover->popdown(); });

    confirm_button->signal_clicked().connect(
            [this, popover]
            {
                popover->popdown();
            });
}

void ProjectHierarchyPane::configure_delete_structure_popover(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "delete_structure_popover");
    const auto confirm_button =
            GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "delete_structure_confirm");
    const auto cancel_button =
            GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "delete_structure_cancel");
    const auto property_path =
            GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "delete_structure_property_path");

    popover->signal_show().connect(
            [this, property_path]
            {
                const auto candidate =
                        std::dynamic_pointer_cast<const TreeNode>(selection_model->get_selected_item());
                if (candidate != nullptr)
                    property_path->set_text(candidate->get_path() + "/*");
            });

    cancel_button->signal_clicked().connect([popover] { popover->popdown(); });

    confirm_button->signal_clicked().connect(
            [this, popover]
            {
                popover->popdown();

                const auto candidate =
                        std::dynamic_pointer_cast<TreeNode>(selection_model->get_selected_item());

                if (candidate != nullptr) {
                    const auto parent = candidate->get_parent();
                    if (parent != nullptr) {
                        // If an owning model is explicitly declared, use that.
                        const auto owning_model = parent->get_children();
                        const auto owning_model_count = owning_model->get_n_items();
                        for (guint idx = 0; idx < owning_model_count; ++idx)
                            if (owning_model->get_item(idx) == candidate)
                                owning_model->remove(idx);
                    } else {
                        // Otherwise, use the root model.
                        const auto root_model_count = data_model->get_n_items();
                        for (guint idx = 0; idx < root_model_count; ++idx)
                            if (data_model->get_item(idx) == candidate)
                                data_model->remove(idx);
                    }
                }

                emit_deselected();
            });
}

void ProjectHierarchyPane::tree_node_setup(const Glib::RefPtr<Gtk::ListItem> &item)
{
    const auto expander = Gtk::make_managed<Gtk::TreeExpander>();
    const auto label = Gtk::make_managed<Gtk::Label>();

    label->set_halign(Gtk::Align::START);
    expander->set_child(*label);
    item->set_child(*expander);
}

void ProjectHierarchyPane::switch_visible_stack() const
{
    switch (stack_switcher->get_selected()) {
    case static_cast<guint>(ProjectStackSwitcherIdx::Explorer):
        stack->set_visible_child("project_explorer");
        break;
    case static_cast<guint>(ProjectStackSwitcherIdx::Metadata):
        stack->set_visible_child("project_metadata");
        break;
    default:
        break;
    }
}

Glib::RefPtr<Gio::ListModel> ProjectHierarchyPane::tree_node_expand(
        const Glib::RefPtr<Glib::ObjectBase> &item)
{
    const auto candidate = std::dynamic_pointer_cast<TreeNode>(item);

    if (candidate != nullptr)
        return candidate->get_children();

    return nullptr;
}

// ReSharper disable once CppDFAUnreachableFunctionCall - false positive
void ProjectHierarchyPane::emit_selected(const Glib::RefPtr<Subsystem> &new_subsystem) const
{
    for (const auto &[select_callback, _]: subsystem_change_callbacks)
        select_callback.emit(new_subsystem);
}

void ProjectHierarchyPane::emit_deselected() const
{
    for (const auto &[_, deselect_callback]: subsystem_change_callbacks)
        deselect_callback.emit();
}

void ProjectHierarchyPane::switch_selection(guint) const
{
    // Rely on the selection model to inform on the selected item. The view can be unreliable.
    const auto subsystem = std::dynamic_pointer_cast<Subsystem>(selection_model->get_selected_item());
    if (subsystem != nullptr)
        emit_selected(subsystem);
    else
        emit_deselected();
}

} // namespace optifol
