/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic IR Term interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef ITERMNODE_HPP
#define ITERMNODE_HPP

#include <memory>
#include <string>

namespace optifol
{

class MutatingTermVisitorBase;

class ITermNode
{
public:
    virtual ~ITermNode() = default;

    [[nodiscard]] virtual std::unique_ptr<ITermNode> clone() const = 0;

    [[nodiscard]] virtual std::string to_string() const = 0;

    [[nodiscard]] virtual std::string get_disambiguated_name() const = 0;

    virtual void accept(MutatingTermVisitorBase& visitor) = 0;
};

}

#endif
