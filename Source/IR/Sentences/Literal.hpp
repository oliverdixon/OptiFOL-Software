/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Literal IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#ifndef LITERAL_HPP
#define LITERAL_HPP

#include <vector>

#include "IProcessedSentence.hpp"

namespace optifol
{

class IProcessedTerm;
class UnificationVisitor;

/**
 * @class Literal
 * @brief A Literal represents a non-owning IR node sentence consisting of a display name and zero to many arguments,
 *  all of which are referenced from the centralised SymbolRepository.
 * @see MutablePredicate for the unprocessed, argument-owning dual.
 */
class Literal : public IProcessedSentence
{
public:
    /**
     * @brief Create a signed predicate with an initial set of referenced arguments
     * @param name Literal display name
     * @param arguments Set of non-owning pointers to immutable arguments
     * @param is_positive Should the predicate be instantiated with a positive polarity?
     */
    explicit Literal(
            std::string name, std::initializer_list<const IProcessedTerm *> arguments = {}, bool is_positive = true);

    /**
     * @brief Create a signed predicate with an initial set of referenced arguments
     * @param name Literal display name
     * @param arguments Set of non-owning pointers to immutable arguments
     * @param is_positive Should the predicate be instantiated with a positive polarity?
     */
    explicit Literal(
            std::string name, std::vector<const IProcessedTerm *>&& arguments, bool is_positive = true);

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    /**
     * @brief Get the display name of the predicate, not including any arguments or metadata
     * @return A view of the predicate symbol name
     */
    [[nodiscard]] std::string_view get_name() const noexcept;

    /**
     * @brief Observe the non-owning ordered argument collection
     * @return The arguments referenced by the predicate
     */
    [[nodiscard]] const std::vector<const IProcessedTerm *> &observe_arguments() const noexcept;

    [[nodiscard]] bool accept(UnificationVisitor &unification_visitor, const IProcessedSentence &sentence) const
        override;

    [[nodiscard]] bool accept(UnificationVisitor &unification_visitor, const Literal &predicate) const override;

private:
    const std::string name;
    const std::vector<const IProcessedTerm *> arguments;
    const bool is_positive = true;
};

} // namespace optifol

#endif
