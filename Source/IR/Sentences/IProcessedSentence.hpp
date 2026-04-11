/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the IProcessedSentence interface
 * @author Oliver Dixon
 * @date 2025-06-07
 * @version Development
 */

#ifndef IPROCESSEDSENTENCE_HPP
#define IPROCESSEDSENTENCE_HPP

#include "ISentence.hpp"

namespace optifol
{

class IObservingBinaryVisitor;
class Literal;

/**
 * @class IProcessedSentence
 * @brief An IProcessedSentence is the interface for immutable sentence AST nodes that have been normalised and are no
 *  longer editable. They're typically stored in a global SymbolRepository and referenced with observing pointers.
 */
class IProcessedSentence : public ISentence
{
public:
    [[nodiscard]] virtual bool accept(
            IObservingBinaryVisitor &binary_visitor, const IProcessedSentence &sentence) const
    {
        std::ignore = binary_visitor;
        std::ignore = sentence;
        return false;
    }

    [[nodiscard]] virtual bool accept(
            IObservingBinaryVisitor &unification_visitor, const Literal &predicate) const
    {
        std::ignore = unification_visitor;
        std::ignore = predicate;
        return false;
    }

    /**
     * @brief Test equality between two IProcessedSentence instances.
     * @param other The other IProcessedSentence instance with which to test for equality.
     * @return Are the two sentences of the same concrete type and equal?
     */
    [[nodiscard]] virtual bool operator==(const IProcessedSentence &other) const noexcept = 0;
};

} // namespace optifol

#endif // IPROCESSEDSENTENCE_HPP
