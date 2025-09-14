/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-06-23
 * @version Development
 */

#include "SentenceRoot.hpp"

#include "Literal.hpp"

namespace optifol
{

bool SentenceRoot::is_negative_polarity() const noexcept
{
    return false;
}

std::ostream &SentenceRoot::serialise(std::ostream &ostream) const
{
    ostream << '{' << ' ';

    if (clauses.empty() == false) {
        const auto clause_last_idx = clauses.size() - 1;
        for (std::size_t clause_idx = 0; clause_idx < clause_last_idx; ++clause_idx) {
            serialise_clause(ostream, clauses[clause_idx]);
            ostream << ',' << ' ';
        }

        serialise_clause(ostream, clauses[clause_last_idx]);
    }

    return ostream << ' ' << '}';
}

std::size_t SentenceRoot::hash() const noexcept
{
    std::size_t hash_value = clauses.size();

    for (const auto& clause : clauses) {
        std::size_t literal_hash_value = clause.size();
        for (const auto literal : clause)
            literal_hash_value = hash_combine(literal_hash_value, literal->hash());

        hash_value = hash_combine_commutative(hash_value, literal_hash_value);
    }

    return hash_value;
}

bool SentenceRoot::operator==(const IProcessedSentence &other) const noexcept
{
    const auto other_root = dynamic_cast<const SentenceRoot *>(&other);
    if (other_root == nullptr)
        // Other IProcessedSentence isn't a SentenceRoot.
        return false;

    const auto clause_count = clauses.size();
    if (clause_count != other_root->clauses.size())
        // Other SentenceRoot has a different number of clauses under conjunction.
        return false;

    for (std::size_t clause_idx = 0; clause_idx < clause_count; ++clause_idx) {
        const auto& clause = clauses[clause_idx];
        const auto& other_clause = other_root->clauses[clause_idx];

        const auto literal_count = clause.size();
        if (literal_count != other_clause.size())
            // Other clause has a different number of literals under disjunction.
            return false;

        for (std::size_t literal_idx = 0; literal_idx < literal_count; ++literal_idx)
            if (clause[literal_idx]->operator==(*other_clause[literal_idx]))
                /*
                 * Other literal is different. This uses Literal::operator==, not memory addresses. Otherwise we could
                 * just use std::vector::operator==(const std::vector&) and forget about all this!
                 */
                return false;
    }

    return true;
}

void SentenceRoot::commit_clause(const std::vector<const Literal *> &literals)
{
    clauses.emplace_back(literals);
}

std::vector<SentenceRoot::Clause>::const_iterator SentenceRoot::begin() const noexcept
{
    return clauses.cbegin();
}

std::vector<SentenceRoot::Clause>::const_iterator SentenceRoot::end() const noexcept
{
    return clauses.cend();
}

std::ostream &SentenceRoot::serialise_clause(std::ostream &ostream, const std::vector<const Literal *> &clause)
{
    ostream << '{' << ' ';

    if (clause.empty() == false) {
        const auto literal_last_idx = clause.size() - 1;
        for (std::size_t literal_idx = 0; literal_idx < literal_last_idx; ++literal_idx) {
            clause[literal_idx]->serialise(ostream);
            ostream << ',' << ' ';
        }

        clause[literal_last_idx]->serialise(ostream);
    }

    return ostream << ' ' << '}';
}

} // namespace optifol
