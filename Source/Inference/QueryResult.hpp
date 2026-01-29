/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL Resolution Knowledge Base
 * @author Oliver Dixon
 * @date 2025-09-14
 * @version Development
 */

#ifndef OPTIFOL_QUERYRESULT_HPP
#define OPTIFOL_QUERYRESULT_HPP

#include <deque>

#include "../IR/Sentences/Clause.hpp"

namespace optifol
{

class Resolvent;

struct QueryResult
{
    enum class ConjectureStatus
    {
        NotExecuted,
        TimedOut,
        Consistent,
        Inconsistent
    };

    QueryResult() = default;

    ConjectureStatus outcome = ConjectureStatus::NotExecuted;
    std::size_t elapsed_step_count = 1;

    UniqueUnorderedSet<Clause> introduced_clauses;
    std::deque<Resolvent> relations;
    const Resolvent * terminating_resolvent = nullptr;

    QueryResult(QueryResult&&) = default;
    QueryResult& operator=(QueryResult&&) = default;
};

} // namespace optifol

#endif // OPTIFOL_QUERYRESULT_HPP
