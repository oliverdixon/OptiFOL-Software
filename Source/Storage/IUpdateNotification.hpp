/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef IUPDATENOTIFICATION_HPP
#define IUPDATENOTIFICATION_HPP

#include <cstddef>

namespace optifol
{

class IUpdateNotification
{
public:
    enum class Action
    {
        Insert,
        Update,
        Delete
    };

    enum class Scope
    {
        Project
    };

    virtual ~IUpdateNotification() = default;

    [[nodiscard]] virtual Action get_action() const = 0;

    [[nodiscard]] virtual Scope get_scope() const = 0;

    [[nodiscard]] virtual std::size_t get_id() const = 0;
};

}

#endif
