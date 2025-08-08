/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the object grouping base
 * @author Oliver Dixon
 * @date 2025-07-26
 * @version Development
 */

#ifndef OBJECTGROUPBASE_HPP
#define OBJECTGROUPBASE_HPP

#include <giomm/liststore.h>
#include <gtkmm/singleselection.h>

#include "../DereferencingEqualityFunctor.hpp"
#include "StorageObjectBase.hpp"

namespace optifol
{

/**
 * @class ObjectGroupBase
 * @brief Provide a common CRTP interface for all structures grouping objects in a mutable list for iteration, but also
 *  require fast lookup. The base provides a skeleton set of observing and mutating operations on the model to make
 *  optimal use of the dual-storage (list and map) model.
 * @tparam Derived The IHashable type to store.
 *
 * @details
 *  The ObjectGroupBase provides two important data structures:
 *  <ul>
 *      <li>Gtk::ListStore of the type. This is a linear Gtk model that can be attached to views.</li>
 *      <li>@ref std::unordered_map of the types, mapped to their positions in the Gtk::ListStore.</li>
 *  </ul>
 *  Duplicating references across two structures provides benefits of Gtk integration through the linear model, and fast
 *  lookup and hashing capability through the @ref std::unordered_map model. The models are internally synchronised
 *  using the <i>libsigc++</i> callbacks provided natively by Gtkmm.
 */
template<typename Derived>
    requires std::derived_from<Derived, IHashable>
class ObjectGroupBase
{
public:
    /**
     * @brief Destruct the ObjectGroupBase.
     */
    virtual ~ObjectGroupBase() = default;

    /**
     * @brief Retrieves the untyped Gio::ListModel from a static context; this is required for GUI integration.
     * @param untyped_item The Glib-enforced argument containing the object representing the object group.
     * @return The Gio::ListModel held by the object group, or the null pointer if no such model could be retrieved.
     */
    static Glib::RefPtr<Gio::ListModel> get_model(const Glib::RefPtr<Glib::ObjectBase> &untyped_item)
    {
        const auto object_group = dynamic_cast<const ObjectGroupBase *>(untyped_item.get());

        if (object_group != nullptr)
            return object_group->model;

        return nullptr;
    }

    /**
     * @brief Insert a new single object into the model.
     * @param new_object The constructed object to insert into the models.
     */
    void insert_object(Glib::RefPtr<Derived> new_object)
    {
        model->append(new_object);
        index_map[new_object] = model->get_n_items() - 1;
    }

    /**
     * @brief Insert multiple objects into the model.
     * @param new_objects The constructed set of objects to insert into the models.
     */
    void insert_object(const std::vector<Glib::RefPtr<Derived>> &new_objects)
    {
        // Append the incoming objects into the list model.
        const auto previous_count = model->get_n_items();
        model->splice(previous_count, 0, new_objects);

        // Verify insertion with the constant-time size heuristic.
        const auto new_count = model->get_n_items();
        assert(new_count == previous_count + new_objects.size());

        // Map each new object to its corresponding position in the list model.
        for (guint new_index = previous_count; new_index < new_count; ++new_index)
            index_map[new_objects[new_count - previous_count]] = new_index - 1;
    }

    /**
     * @brief Take a ref-counted pointer of the object at the given index.
     * @param object_index The index of the object within the linear model to take.
     * @return The requested object wrapped in a ref-counted Glib::RefPtr.
     * @throws std::runtime_error An object did not exist in the linear model at the specified index.
     */
    Glib::RefPtr<Derived> get_object_by_index(guint object_index) const
    {
        const auto object = model->get_item(object_index);

        if (object == nullptr)
            throw std::runtime_error("Object with given index does not exist in the Subsystem index.");

        return object;
    }

    /**
     * @brief Delete the given object from the model.
     * @param slated_object The object to delete.
     * @return Was an object deleted?
     */
    bool delete_object(const Glib::RefPtr<Derived> &slated_object)
    {
        // Locate the object in the map, which will provide its index in the linear model.
        const auto index_it = index_map.find(slated_object);
        if (index_it == index_map.cend())
            return false;

        /*
         * Record as a deletion and remove from the linear model. The removal from the linear model will likely trigger
         * a user-defined callback to indicate the removal; the removed object can be retrieved from the callback with
         * 'steal_deleted_object'.
         */
        pending_deleted_items[index_it->second] = std::move(slated_object);
        model->remove(index_it->second);

        // Recalculate the index map.
        for (auto &it: index_map)
            /*
             * TODO this is very inefficient. We should maintain a shift map to correspond offsets from the index map to
             *  be applied only when required. Should be done with reasonable urgency. We're reindexing the entire map
             *  for every deletion!
             *
             * Sketch of a solution:
             *
             * Keep an ordered map to maintain a correspondence between the index in the index map ("stored index") and
             * shifts that have been applied to that elements. If you had A, B, and C; and B was deleted; the stored
             * index of C would be 2 but we could record a shift of -1. When the index of C was queried again, we would
             * apply the shift lazily.
             *
             * Of course, changes need to cascade. So if A was also deleted, a shift of -1 would be recorded for B, but
             * the C shift should be updated to -2. When the shift map became sufficiently large, an entire re-index
             * could be done to clear down the shift records.
             *
             * I did attempt this but it was finicky, and wasn't completely clear on the problem I was trying to solve.
             * Needs a second attempt soon.
             */
            if (it.second > index_it->second)
                --it.second;

        index_map.erase(index_it);
        return true;
    }

    /**
     * @brief Steal a recently deleted object from the cache. After this operation, the object is no longer present in
     *  any of the models, including the deletion cache.
     * @param old_index The index of the object in the linear model prior to its deletion.
     * @return The deleted object.
     * @throws std::runtime_error if no suitable object exists in the deletion cache.
     */
    Glib::RefPtr<Derived> steal_deleted_object(const guint old_index)
    {
        const auto deleted_it = pending_deleted_items.find(old_index);

        if (deleted_it == pending_deleted_items.cend())
            throw std::runtime_error("Requested object at index " + std::to_string(old_index) + " does not exist in "
                "the deletion map.");

        const auto copy = deleted_it->second;
        pending_deleted_items.erase(deleted_it);
        return copy;
    }

    /**
     * @brief Populate the given Gtk::SingleSelection selection model with the internal model.
     * @param target_selection_model The selection model to populate with the current list model.
     */
    void populate_selection_model(Gtk::SingleSelection &target_selection_model) const
    {
        target_selection_model.set_model(model);
    }

    /**
     * @brief Execute a callable over all elements of the linear model.
     * @param function The callable to execute, taking a mutating reference to the object.
     */
    void for_each(const std::function<void(Derived &)> &function) const
    {
        const auto count = model->get_n_items();
        for (guint index = 0; index < count; ++index)
            function(*model->get_item(index));
    }

protected:
    /**
     * @brief Construct a ObjectGroupBase.
     */
    ObjectGroupBase()
    {
        model->signal_items_changed().connect(sigc::mem_fun(*this, &ObjectGroupBase::handle_object_change));
    }

    /**
     * @brief Handle insertions and/or deletions in the flat linear model.
     * @param initial_index The index at which insertions/deleted started.
     * @param removed_count The number of objects removed from the list.
     * @param added_count The number of objects added to the list.
     */
    virtual void handle_object_change(const guint initial_index,
        const guint removed_count, const guint added_count) noexcept
    {
        std::ignore = initial_index;
        std::ignore = removed_count;
        std::ignore = added_count;
    }

    Glib::RefPtr<Gio::ListStore<Derived>> model = Gio::ListStore<Derived>::create();

private:
    /**
     * @brief Mapping of hashable objects present in the models, associated with their respective indices in the linear
     *  @ref model.
     */
    std::unordered_map<Glib::RefPtr<Derived>, guint, std::hash<Derived>,
            DereferencingEqualityFunctor<Glib::RefPtr<Derived>, Derived>>
            index_map;

    /**
     * @brief Items deleted from the linear @ref model but not the @ref index_map. Keys indicate the old positions of
     *  the values in @ref model.
     */
    std::unordered_map<guint, Glib::RefPtr<Derived>> pending_deleted_items;
};

} // namespace optifol

#endif // OBJECTGROUPBASE_HPP
