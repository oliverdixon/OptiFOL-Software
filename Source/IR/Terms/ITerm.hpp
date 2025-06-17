/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic IR Term interface
 * @author Oliver Dixon
 * @date 2025-06-14
 * @version Development
 */

#ifndef ITERM_HPP
#define ITERM_HPP

#include "../../IHashable.hpp"

namespace optifol
{

class UnificationVisitor;
class Variable;
class Function;

class ITerm :
        public IHashable
{
public:
    [[nodiscard]] virtual std::string to_string() const = 0;

    [[nodiscard]] virtual std::string get_disambiguated_name() const = 0;

    [[nodiscard]] std::size_t hash() const noexcept override
    {
        return std::hash<std::string>{}(to_string());
    }

    virtual std::ostream &serialise(std::ostream &ostream) const
    {
        return ostream << to_string();
    }

    bool operator==(const ITerm &other) const
    {
        return hash() == other.hash();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const ITerm& object)
    {
        return object.serialise(ostream);
    }

    bool operator==(const std::unique_ptr<ITerm>& other) const
    {
        return other->hash() == hash();
    }

    bool operator==(const std::shared_ptr<ITerm>& other) const
    {
        return other->hash() == hash();
    }
};

}

#endif
