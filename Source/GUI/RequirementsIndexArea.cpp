/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "RequirementsIndexArea.hpp"

namespace
{

template<typename F>
struct const_mem_fun_return_type;

// TODO: this is unbelievably niche and useless. Document it as such!
template<typename ClassType, typename ReturnType, typename... Args>
struct const_mem_fun_return_type<ReturnType (ClassType::*)(Args...) const>
{
    using type = ReturnType;
};

}

namespace optifol
{

RequirementsIndexArea::RequirementsIndexArea(Gtk::ColumnView *view,
        const Glib::RefPtr<PGRequirementModel> &initial_model) :
    requirements_model(initial_model)
{
    const auto selection_model = Gtk::SingleSelection::create(initial_model);
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
                factory->signal_setup().connect(sigc::ptr_fun(&RequirementsIndexArea::on_setup_static));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_name(list_item, &Requirement::get_identifier); });
            } else if (gtk_id == "requirement_statement") {
                factory->signal_setup().connect(sigc::ptr_fun(&RequirementsIndexArea::on_setup_statement));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_name(list_item, &Requirement::get_statement); });
            } else if (gtk_id == "requirement_priority") {
                factory->signal_setup().connect(sigc::ptr_fun(&RequirementsIndexArea::on_setup_statement));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                    { return on_bind_name(list_item, &Requirement::get_priority); });
            } else
                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
            ++processed_columns;
        }
    }

#if 0
    if (processed_columns < column_count)
        /*
         * TODO: log. Probably should be an exception, since we cannot show the index if all columns aren't initialised
         *  and hooked up to factories.
         */
        assert(0);
#endif
}

void RequirementsIndexArea::on_setup_static(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = Gtk::make_managed<Gtk::Label>();
    label->set_halign(Gtk::Align::START);
    list_item->set_child(*label);
}

void RequirementsIndexArea::on_setup_statement(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = Gtk::make_managed<Gtk::Label>();
    label->set_halign(Gtk::Align::START);
    label->set_css_classes({"optifol_fol_statement"});
    list_item->set_child(*label);
}

void RequirementsIndexArea::on_setup_edit(const Glib::RefPtr<Gtk::ListItem> &list_item,
                                          Glib::SignalProxyProperty::SlotType &&edit_callback)
{
}

template<typename F>
void RequirementsIndexArea::on_bind_name(const Glib::RefPtr<Gtk::ListItem> &list_item, F &&information_function) const
{
    const auto position = list_item->get_position();

    if (position == GTK_INVALID_LIST_POSITION)
        return;

    const auto model_item = requirements_model->get_typed_object<Requirement>(position);
    if (!model_item)
        return;

    const auto label = dynamic_cast<Gtk::Label*>(list_item->get_child());
    if (!label)
        return;

    if constexpr (std::is_convertible_v<Glib::ustring, typename const_mem_fun_return_type<F>::type>)
        // If the information function will give us something convertible to a GTK string, just pass it through.
        label->set_text(std::invoke(std::forward<F>(information_function), model_item));
    else
        // Otherwise, see if std::to_string has an overload. This is typically useful for standard-typed numbers.
        label->set_text(std::to_string(std::invoke(std::forward<F>(information_function), model_item)));
}

}
