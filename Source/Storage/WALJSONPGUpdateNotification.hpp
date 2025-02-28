/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef WALJSONPGUPDATENOTIFICATION_HPP
#define WALJSONPGUPDATENOTIFICATION_HPP

namespace optifol
{

class WALJSONPGUpdateNotification
{
public:
    enum class Action
    {
        NoOp,
        Insert,
        Update,
        Delete
    };

    enum class Scope
    {
        Empty,
        Project,
        Subsystem
    };

    std::size_t id;

    Action action{Action::NoOp};

    Scope scope{Scope::Empty};
};

}

namespace simdjson
{

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
    } else if (change_kind == "update") {
        payload.action = optifol::WALJSONPGUpdateNotification::Action::Update;
        payload.id = object["columnvalues"].at(0);
    } else if (change_kind == "delete") {
        payload.action = optifol::WALJSONPGUpdateNotification::Action::Delete;
        payload.id = object["oldkeys"]["keyvalues"].at(0);
    } else
        return error_code::STRING_ERROR;

    return error_code::SUCCESS;
}

}

#endif
