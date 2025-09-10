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

#include "IProcessedSentence.hpp"

namespace optifol
{

/**
 * @class SentenceRoot
 * @brief A SentenceRoot denotes the root node of an immutable IR node set. It contains conjunctive clauses of literals
 *  under disjunction, where the literals are weak references to a lifetime-assured SymbolRepository.
 */
class SentenceRoot : public IProcessedSentence
{
    std::vector<std::vector<const Literal *>> clauses;

public:
    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    std::ostream& serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    void commit_clause(const std::vector<const Literal *> &literals);

    decltype(clauses)::const_iterator cbegin() const noexcept;

    decltype(clauses)::const_iterator cend() const noexcept;

private:
    static std::ostream& serialise_clause(std::ostream &ostream, const std::vector<const Literal *> &clause);
};

} // namespace optifol

#endif // SENTENCEROOT_HPP
