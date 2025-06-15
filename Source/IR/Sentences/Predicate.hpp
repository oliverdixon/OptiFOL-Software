/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Predicate IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <vector>

#include "ISentence.hpp"

namespace optifol
{

class IProcessedTerm;
class UnificationVisitor;

/**
 * @class Predicate
 * @brief A Predicate represents a non-owning IR node sentence consisting of a display name and zero to many arguments,
 *  all of which are referenced from the centralised SymbolRepository.
 * @see MutablePredicate for the unprocessed, argument-owning dual.
 */
class Predicate :
        public ISentence
{
public:
    /**
     * @brief Create a signed predicate with an initial set of referenced parameters
     * @param name Predicate display name
     * @param is_positive Should the predicate be instantiated with a positive polarity?
     * @param arguments Set of non-owning pointers to immutable arguments
     */
    explicit Predicate(std::string name, bool is_positive = true, std::initializer_list<const IProcessedTerm *>
        arguments = {});

    /**
     * @brief Create a signed predicate with an initial set of referenced arguments
     * @param name Predicate display name
     * @param arguments Set of non-owning pointers to immutable arguments
     * @param is_positive Should the predicate be instantiated with a positive polarity?
     */
    explicit Predicate(std::string name, std::initializer_list<const IProcessedTerm *> arguments = {},
        bool is_positive = true);

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    /**
     * @brief Get the display name of the predicate, not including any arguments or metadata
     * @return A view of the predicate symbol name
     */
    std::string_view get_name() const noexcept;

    /**
     * @brief Observe the non-owning ordered argument collection
     * @return The arguments referenced by the predicate
     */
    [[nodiscard]] const std::vector<const IProcessedTerm *> &observe_arguments() const noexcept;

    /**
     * @brief Accept a visitation from a UnificationVisitor for predicate-unification.
     * @param visitor The instance of a UnificationVisitor to use
     * @param target The Predicate with which unification should be attempted
     * @return Did the UnificationVisitor report a successful unification?
     * @see TransparentlyHashable for rationale
     */
    bool accept(UnificationVisitor &visitor, const Predicate &target) const;

    /**
     * @brief Compare with another predicate wrapped in a @ref std::unique_ptr
     * @param other The wrapper containing the predicate against which equality should be determined
     * @return Is the wrapped predicate hash-equal to us?
     * @see GoogleTestable for desired concept
     */
    bool operator==(const std::unique_ptr<Predicate>& other) const noexcept
    {
        return other->hash() == hash();
    }

    /**
     * @brief Compare with another predicate wrapped in a @ref std::shared_ptr
     * @param other The wrapper containing the predicate against which equality should be determined
     * @return Is the wrapped predicate hash-equal to us?
     * @see TransparentlyHashable for rationale
     */
    bool operator==(const std::shared_ptr<Predicate>& other) const noexcept
    {
        return other->hash() == hash();
    }

private:
    const std::string name;

    std::vector<const IProcessedTerm *> arguments;

    bool is_positive = true;
};

}

#endif
