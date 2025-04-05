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
     * Default virtual destructor
     */
    virtual ~ModelPublisherBase() = default;

    /**
     * @brief Add a new subscriber to listen for insertions on the current model instance
     * @param slot The callback exposed by the subscriber's API
     * @param onboard Ignored for the storage-agnostic base implementation
     * @return The newly added signal
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
     * @brief Inform all insert-subscribers of a new insertion to the model
     * @param inserted_item A copy of the ref-counted pointer holding the newly inserted item
     */
    void inform_insertion(Glib::RefPtr<Type> inserted_item) const
    {
        for (const auto& signal : insert_callbacks)
            signal(std::move(inserted_item));
    }

private:
    std::vector<sigc::signal<InsertionCallbackSignature>> insert_callbacks;
};

}

#endif
