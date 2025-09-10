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

void SentenceRoot::commit_clause(const std::vector<const Literal *> &literals)
{
    clauses.emplace_back(literals);
}

decltype(SentenceRoot::clauses)::const_iterator SentenceRoot::cbegin() const noexcept
{
    return clauses.cbegin();
}

decltype(SentenceRoot::clauses)::const_iterator SentenceRoot::cend() const noexcept
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
