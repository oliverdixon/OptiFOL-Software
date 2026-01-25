/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-06-23
 * @version Development
 */

#ifndef SENTENCEROOT_HPP
#define SENTENCEROOT_HPP

#include "Clause.hpp"
#include "IProcessedSentence.hpp"

namespace optifol
{

/**
 * @class SentenceRoot
 * @brief A SentenceRoot denotes the root node of an immutable IR node set. It contains conjunctive clauses of literals
 *  under disjunction, where the literals are weak references to a lifetime-assured SymbolRepository.
 */
class SentenceRoot : public IProcessedSentence, public OwningBuildable<SentenceRoot>
{
public:
    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    std::ostream& serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    [[nodiscard]] bool operator==(const IProcessedSentence &other) const noexcept override;

    void add_clause(const Clause &new_clause);

    [[nodiscard]] std::vector<Clause>::const_iterator begin() const noexcept;

    [[nodiscard]] std::vector<Clause>::const_iterator end() const noexcept;

    [[nodiscard]] std::size_t order() const noexcept;

private:
    std::vector<Clause> clauses;

    bool is_negative = false;
};

} // namespace optifol

#endif // SENTENCEROOT_HPP
