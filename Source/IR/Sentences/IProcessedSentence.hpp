/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/6/25.
//

#ifndef IPROCESSEDSENTENCE_HPP
#define IPROCESSEDSENTENCE_HPP

#include "ISentence.hpp"

namespace optifol
{

class IObservingBinaryVisitor;
class Literal;

class IProcessedSentence : public ISentence
{
public:
    [[nodiscard]] virtual bool accept(IObservingBinaryVisitor &binary_visitor, const IProcessedSentence &sentence) const
    {
        std::ignore = binary_visitor;
        std::ignore = sentence;
        return false;
    }

    [[nodiscard]] virtual bool accept(IObservingBinaryVisitor &unification_visitor, const Literal &predicate) const
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
