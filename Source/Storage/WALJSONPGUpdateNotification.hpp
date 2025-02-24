/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef WALJSONPGUPDATENOTIFICATION_HPP
#define WALJSONPGUPDATENOTIFICATION_HPP

#include <optional>

#include "IUpdateNotification.hpp"

namespace optifol
{

class WALJSONPGUpdateNotification :
        public IUpdateNotification
{
public:
    [[nodiscard]] Action get_action() const override;

    [[nodiscard]] Scope get_scope() const override;

    [[nodiscard]] std::size_t get_id() const override;

    std::optional<std::size_t> id;

    std::optional<Action> action;

    std::optional<Scope> scope;
};

}

namespace simdjson
{

template<typename simdjson_value>
error_code tag_invoke(deserialize_tag, simdjson_value &value, optifol::WALJSONPGUpdateNotification &payload)
{
    ondemand::object object;
    value.get_object().get(object);

    auto change_kind = object["kind"];

    if (object["table"] == "project")
        payload.scope = optifol::IUpdateNotification::Scope::Project;

    if (change_kind == "insert") {
        payload.action = optifol::IUpdateNotification::Action::Insert;
        payload.id = object["columnvalues"].at(0);
    } else if (change_kind == "update") {
        payload.action = optifol::IUpdateNotification::Action::Update;
        payload.id = object["columnvalues"].at(0);
    } else if (change_kind == "delete") {
        payload.action = optifol::IUpdateNotification::Action::Delete;
        payload.id = object["oldkeys"]["keyvalues"].at(0);
    }

    return error_code::SUCCESS;
}

}

#endif
