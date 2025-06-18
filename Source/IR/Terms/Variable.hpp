/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Variable IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <optional>

#include "IProcessedTerm.hpp"

namespace optifol
{

/**
 * @class Variable
 * @brief A Variable is a non-owning IR node representing a first-order logic non-free variable.
 * @see MutableVariable for the owning, mutable dual; MutableVariable also contains more documentation of the semantics
 *  of an Optifol first-order logic variable.
 */
class Variable : public IProcessedTerm
{
public:
    /**
     * @brief Create a new Variable with a fixed display name
     * @param name The fixed name of the variable
     */
    explicit Variable(std::string name);

    /**
     * @brief Create a new Variable with a fixed display name and disambiguated name
     * @param name The fixed name of the variable
     * @param disambiguated_name The fixed disambiguated name for the variable
     * @warning No uniqueness check is done for the disambiguated name upon construction
     */
    explicit Variable(std::string name, const std::string &disambiguated_name);

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string_view get_disambiguated_name() const override;

private:
    const std::string name;
    const std::optional<std::string> disambiguated_name;
};

} // namespace optifol

#endif
