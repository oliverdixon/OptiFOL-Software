/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Requirement grouping base
 * @author Oliver Dixon
 * @date 2025-07-26
 * @version Development
 */

#ifndef REQUIREMENTGROUPBASE_HPP
#define REQUIREMENTGROUPBASE_HPP

#include <giomm/liststore.h>
#include <gtkmm/singleselection.h>

#include "../DereferencingEqualityFunctor.hpp"
#include "Requirement.hpp"

namespace optifol
{

/**
 * @class RequirementGroupBase
 * @brief Provide a common interface for all structures grouping Requirement objects in a mutable list for iteration,
 *  but also require fast lookup. The base provides a skeleton set of observing and mutating operations on the model to
 *  make optimal use of the dual-storage (list and map) model.
 */
class RequirementGroupBase
{
public:
    /**
     * @brief Construct a RequirementGroupBase without a callback for model changes.
     */
    RequirementGroupBase() = default;

    /**
     * @brief Construct a RequirementGroupBase with a callback to indicate insertions and/or deletions from a fixed
     *  point in the list.
     * @param changed_callback The slot for a handler to process insertions and/or deletions. Arguments provided are the
     *  fixed initial list index; the number of removed elements; and the number of added elements.
     */
    explicit RequirementGroupBase(sigc::slot<void(guint, guint, guint)> &&changed_callback);

    /**
     * @brief Destruct the RequirementGroupBase.
     */
    virtual ~RequirementGroupBase() = default;

    template<class Grouping> requires std::derived_from<Grouping, RequirementGroupBase>
    static Glib::RefPtr<Gio::ListModel> get_expanded_list(const Glib::RefPtr<Glib::ObjectBase> &item)
    {
        const auto candidate = std::dynamic_pointer_cast<Grouping>(item);

        if (candidate != nullptr)
            return candidate->get_tree_model();

        return nullptr;
    }

    /**
     * @brief Insert a new Requirement into the model.
     * @param new_requirement The constructed Requirement to insert into the model.
     */
    void insert_requirement(const Glib::RefPtr<Requirement> &new_requirement);

    /**
     * @brief Insert multiple Requirements into the model.
     * @param new_requirements The constructed set of Requirements to insert into the model.
     */
    void insert_requirement(const std::vector<Glib::RefPtr<Requirement>> &new_requirements);

    /**
     * @brief Take a ref-counted pointer of the Requirement at the given index.
     * @param requirement_idx The index of the Requirement within the model to take.
     * @return The requested Requirement wrapped in a ref-counted Glib::RefPtr.
     * @throws std::runtime_error A Requirement did not exist in the model at the specified index.
     */
    Glib::RefPtr<Requirement> get_requirement(guint requirement_idx) const;

    /**
     * @brief Delete the given Requirement from the model.
     * @param slated_requirement The Requirement to delete.
     * @throws std::runtime_error The given Requirement was not within the model.
     */
    void delete_requirement(const Glib::RefPtr<Requirement> &slated_requirement);

    /**
     * @brief Record a Requirement as being slated for deletion by storing a ref-counted pointer to the Requirement
     *  object, and its old index within the flat list model.
     * @param slated_requirement The Requirement being deleted.
     * @param old_index The index of the Requirement in the Requirements Index model immediately prior to deletion.
     */
    virtual void record_slated_requirement(Glib::RefPtr<Requirement> slated_requirement, guint old_index);

    /**
     * @brief Populate the given Gtk::SingleSelection selection model with the internal model.
     * @param target_selection_model The selection model to populate with the current list model.
     */
    void populate_selection_model(Gtk::SingleSelection &target_selection_model) const;

    /**
     * @brief Execute a callable over all elements of the ordered Requirement model.
     * @param function The callable to execute, taking a mutating reference to the Requirement.
     */
    void for_each(const std::function<void(Requirement &)> &function) const;

protected:
    Glib::RefPtr<Gio::ListStore<Requirement>> model = Gio::ListStore<Requirement>::create();

private:
    virtual Glib::RefPtr<Gio::ListModel> get_tree_model() const noexcept;

    std::unordered_map<Glib::RefPtr<Requirement>, guint, std::hash<Requirement>,
            DereferencingEqualityFunctor<Glib::RefPtr<Requirement>, Requirement>>
            index_map;
};

} // namespace optifol

#endif // REQUIREMENTGROUPBASE_HPP
