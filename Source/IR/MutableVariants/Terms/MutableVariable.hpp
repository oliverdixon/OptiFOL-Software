/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Variable Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEVARIABLE_HPP
#define MUTABLEVARIABLE_HPP

#include <optional>

#include "../OwningBuildable.hpp"
#include "IMutableTerm.hpp"

namespace optifol
{

/**
 * @class MutableVariable
 * @brief A MutableVariable is an owning IR node representing a first-order logic non-free variable. Within the
 *  semantics of Optifol, it must be used in the context of a MutableQuantified IR node. Variables have display names
 *  and disambiguated names, which may or may not be identical, although the disambiguated name is unique in the scope
 *  of the outermost sentence.
 * @see Variable for the non-owning, immutable dual
 */
class MutableVariable : public IMutableTerm, public OwningBuildable<MutableVariable>
{
public:
    /**
     * @brief Create a new MutableVariable with an initial display name
     * @param name The initial name of the variable
     */
    explicit MutableVariable(std::string name);

    /**
     * @brief Create a new MutableVariable with an initial display name and disambiguated name
     * @param name The initial name of the variable
     * @param disambiguated_name The initial disambiguated name for the variable
     * @warning No uniqueness check is done for the disambiguated name upon construction
     */
    explicit MutableVariable(std::string name, const std::string &disambiguated_name);

    [[nodiscard]] std::unique_ptr<IMutableTerm> clone() const override;

    void accept(MutatingTermVisitorBase &visitor) override;

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string_view get_disambiguated_name() const override;

private:
    std::string name;
    std::optional<std::string> disambiguated_name;
};

} // namespace optifol

#endif
