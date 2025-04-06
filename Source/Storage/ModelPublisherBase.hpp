/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for models involved publishing signals
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#ifndef MODELPUBLISHERBASE_HPP
#define MODELPUBLISHERBASE_HPP

#include <sigc++/signal.h>

#include "IStorageObject.hpp"

namespace optifol
{

/**
 * @class ModelPublisherBase
 * @brief Provides a base class for storable object models capable of acting as a Publisher, whereby mutations to the
 *  instance are "published" through callbacks to a set of subscribers.
 * @tparam Type The concrete type of the objects to be stored
 * @todo Investigate sigc::scoped_connection. Should we be using that for safety?
 */
template<StorableType Type>
class ModelPublisherBase
{
public:
    /**
     * @typedef InsertionCallbackSignature
     * @brief The function signature of the callback used to inform subscribers of insertions
     * @details For the insertion case, the function should return no value and accept only an r-value reference to a
     *  shared ref-counted pointer of the concrete type managed by the instance.
     */
    using InsertionCallbackSignature = void(Glib::RefPtr<Type>&&);

    /**
     * @typedef UpdateCallbackSignature
     * @brief The function signature of the callback used to inform subscribers of updates
     * @details The callback should take an r-value to a ref-counted pointer of the updated model element.
     */
    using UpdateCallbackSignature = void(Glib::RefPtr<Type>&&);

    /**
     * @typedef DeletionCallbackSignature
     * @brief The function signature of the callback used to inform subscribers of deletions
     * @details The callback should take a single numerical as the controller ID of the deleted object.
     */
    using DeletionCallbackSignature = void(std::size_t);

    /**
     * Default virtual destructor
     */
    virtual ~ModelPublisherBase() = default;

    /**
     * @brief Add a new subscriber to listen for insertions on the current model instance
     * @param slot The callback exposed by the subscriber's API
     * @param onboard Should the instance immediately send signals to the new subscriber for each model item?
     * @return The newly added signal
     * @warning The 'onboard' option requires implementors to override this function to inject their own emplacement
     *  logic. In the base implementation, it does nothing.
     */
    virtual const sigc::signal<InsertionCallbackSignature>& add_insert_subscriber(
        sigc::slot<InsertionCallbackSignature>&& slot, const bool onboard)
    {
        std::ignore = onboard;
        auto& signal = insert_callbacks.emplace_back();
        signal.connect(std::move(slot));
        return signal;
    }

    /**
     * @brief Add a new subscriber to listen for updates on the current model instance
     * @param slot The callback exposed by the subscriber's API
     * @return The newly added signal
     */
    virtual const sigc::signal<UpdateCallbackSignature>& add_update_subscriber(
        sigc::slot<UpdateCallbackSignature>&& slot)
    {
        auto& signal = update_callbacks.emplace_back();
        signal.connect(std::move(slot));
        return signal;
    }

    /**
     * @brief Add a new subscriber to listen for deletions on the current model instance
     * @param slot The callback exposed by the subscriber's API
     * @return The newly added signal
     */
    virtual const sigc::signal<DeletionCallbackSignature>& add_delete_subscriber(
        sigc::slot<DeletionCallbackSignature>&& slot)
    {
        auto& signal = delete_callbacks.emplace_back();
        signal.connect(std::move(slot));
        return signal;
    }

    /**
     * @brief Inform all insert-subscribers of a new insertion to the model
     * @param inserted_item A copy of the ref-counted pointer holding the newly inserted item
     */
    void inform_insertion(Glib::RefPtr<Type> inserted_item) const
    {
        for (const auto& signal : insert_callbacks)
            signal(std::move(inserted_item));
    }

    /**
     * @brief Inform all update-subscribers of a new update from the model
     * @param updated_item A copy of the ref-counted pointer holding the newly updated item
     */
    void inform_update(Glib::RefPtr<Type> updated_item) const
    {
        for (const auto& signal : update_callbacks)
            signal(std::move(updated_item));
    }

    /**
     * @brief Inform all delete-subscribers of a new deletion from the model
     * @param deleted_item_id The controller ID of the deleted item
     */
    void inform_deletion(const std::size_t deleted_item_id) const
    {
        for (const auto& signal : delete_callbacks)
            signal(deleted_item_id);
    }

private:
    std::vector<sigc::signal<InsertionCallbackSignature>> insert_callbacks;

    std::vector<sigc::signal<UpdateCallbackSignature>> update_callbacks;

    std::vector<sigc::signal<DeletionCallbackSignature>> delete_callbacks;
};

}

#endif
