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

class UnificationVisitor;
class Predicate;

class IProcessedSentence : public ISentence
{
public:
    [[nodiscard]] virtual bool accept(UnificationVisitor &unification_visitor, const IProcessedSentence &sentence) const
    {
        std::ignore = unification_visitor;
        std::ignore = sentence;
        return false;
    }

    [[nodiscard]] virtual bool accept(UnificationVisitor &unification_visitor, const Predicate &predicate) const
    {
        std::ignore = unification_visitor;
        std::ignore = predicate;
        return false;
    }
};

} // namespace optifol

#endif // IPROCESSEDSENTENCE_HPP
