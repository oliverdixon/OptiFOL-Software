/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include <simdjson.h>

#include "WALJSONPGUpdateNotification.hpp"
#include "../Exceptions/BadStorageNotificationException.hpp"

namespace optifol
{

IUpdateNotification::Action WALJSONPGUpdateNotification::get_action() const
{
    if (!action)
        throw BadStorageNotificationException("Action type was missing from the PostgreSQL WAL JSON message");

    return *action;
}

IUpdateNotification::Scope WALJSONPGUpdateNotification::get_scope() const
{
    if (!scope)
        throw BadStorageNotificationException("Scope extent was missing from the PostgreSQL WAL JSON message");

    return *scope;
}

std::size_t WALJSONPGUpdateNotification::get_id() const
{
    if (!scope)
        throw BadStorageNotificationException("Propagated ID was missing from the PostgreSQL WAL JSON message");

    return *id;
}

}
