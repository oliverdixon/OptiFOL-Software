/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the WAL JSON PostgreSQL Update Notification container
 * @author Oliver Dixon
 * @date 2025-02-28
 * @version Development
 */

#ifndef WALJSONPGUPDATENOTIFICATION_HPP
#define WALJSONPGUPDATENOTIFICATION_HPP

#include <simdjson.h>

namespace optifol
{

/**
 * @class WALJSONPGUpdateNotification
 * @brief Contains important information from an update notification issued by the PostgreSQL WAL slot
 */
class WALJSONPGUpdateNotification
{
public:
    /**
     * @enum Action
     * @brief The nature of the SQL update
     */
    enum class Action
    {
        NoOp, /**< No substantive update from the DB */
        Insert, /**< An SQL INSERT-like command, requiring an object load */
        Update, /**< An SQL UPDATE-like command, requiring an object reload */
        Delete /**< An SQL DELETE-like command, requiring an object unload */
    };

    /**
     * @enum Scope
     * @brief The scope of the update, isomorphic to a StorableType
     */
    enum class Scope
    {
        Empty, /**< Unrelated to entities tracked by the IStorageController implementation */
        Project, /**< Related to a Project-level entity */
        Subsystem /**< Related to a Subsystem-level entity */
    };

    /**
     * @brief The primary key of the impacted row
     */
    std::size_t id;

    /**
     * @brief An optional slot for a contextually relevant foreign key
     */
    std::optional<std::size_t> associated_fk;

    /**
     * @brief The nature of the SQL update
     */
    Action action{Action::NoOp};

    /**
     * @brief The scope of the update, isomorphic to a StorableType
     */
    Scope scope{Scope::Empty};
};

}

namespace simdjson
{

/**
 * @brief Provide first-class optifol::WALJSONPGUpdateNotification parsing support in SIMDJSON.
 * @tparam simdjson_value The type of the incoming SIMDJSON value
 * @param value The SIMDJSON value extracted by the document iterator
 * @param payload The output parameter for the parsed notification payload
 * @return Success?
 */
template<typename simdjson_value>
error_code tag_invoke(deserialize_tag, simdjson_value &value, optifol::WALJSONPGUpdateNotification &payload)
{
    auto object = value.get_object();
    const auto table_name = object["table"].get_string().value();

    if (table_name == "project")
        payload.scope = optifol::WALJSONPGUpdateNotification::Scope::Project;
    else if (table_name == "subsystem")
        payload.scope = optifol::WALJSONPGUpdateNotification::Scope::Subsystem;
    else
        return error_code::STRING_ERROR;

    const auto change_kind = object["kind"].get_string().value();

    if (change_kind == "insert") {
        payload.action = optifol::WALJSONPGUpdateNotification::Action::Insert;
        payload.id = object["columnvalues"].at(0);

        if (table_name == "subsystem") // TODO: what the fuck?
            payload.associated_fk = object["columnvalues"].at(1);
    } else if (change_kind == "update") {
        payload.action = optifol::WALJSONPGUpdateNotification::Action::Update;
        payload.id = object["columnvalues"].at(0);

        if (table_name == "subsystem") // TODO: what the fuck?
            payload.associated_fk = object["columnvalues"].at(1);
    } else if (change_kind == "delete") {
        payload.action = optifol::WALJSONPGUpdateNotification::Action::Delete;
        payload.id = object["oldkeys"]["keyvalues"].at(0);

        if (table_name == "subsystem") // TODO: what the fuck? Also we need REPLICA IDENTITY FULL set. Any other way?
            payload.associated_fk = object["oldkeys"]["keyvalues"].at(1);
    } else
        return error_code::STRING_ERROR;

    return error_code::SUCCESS;
}

}

#endif
