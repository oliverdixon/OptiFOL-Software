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
#include "../../ISerialisable.hpp"

namespace optifol
{

class UnificationVisitor;
class Variable;
class Function;

/**
 * @class ITerm
 * @brief The ITerm interface describes a generic term in first-order logic.
 * @details
 *  <p>
 *      Typical FOL terms include:
 *      <ul>
 *          <li>Functions (inc. introduced Skolem functions)</li>
 *          <li>Bound variables</li>
 *          <li>Constants</li>
 *      </ul>
 *  </p>
 *  <p>
 *      All terms contain a display name, which is guaranteed to match the symbol identifier provided by the user. Terms
 *      also include a disambiguated name, which may or may not be equal to the display name and is resolved internally
 *      to guarantee uniqueness within the scope of a single sentence. More precisely, the disambiguated name is unique
 *      up to the outermost IR node, but not necessarily across multiple SentenceRoot or ITerm root objects.
 *  </p>
 */
class ITerm : public IHashable, public ISerialisable
{
public:
    [[nodiscard]] std::size_t hash() const noexcept override
    {
        return std::hash<std::string>{}(to_string());
    }

    /**
     * @brief Create a human-readable @ref std::string representing the term, including all child terms.
     * @return The constructed string representation of the term
     */
    [[nodiscard]] virtual std::string to_string() const = 0;

    /**
     * @brief Create a view of the disambiguated term identifier
     * @return The observing string view of the disambiguated name
     */
    [[nodiscard]] virtual std::string_view get_disambiguated_name() const = 0;

    /**
     * @brief Serialise the ITerm to a destination output @ref std::ostream stream
     * @param ostream The destination output stream
     * @return The populated destination output stream
     */
    std::ostream &serialise(std::ostream &ostream) const override
    {
        return ostream << to_string();
    }

    /**
     * @brief Test hash-based equality with another ITerm
     * @param other The ITerm with which equivalence should be tested
     * @return Are the ITerm objects equal?
     */
    bool operator==(const ITerm &other) const
    {
        return hash() == other.hash();
    }
};

} // namespace optifol

#endif
