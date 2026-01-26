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

#ifndef OPTIFOL_KNOWLEDGEBASE_HPP
#define OPTIFOL_KNOWLEDGEBASE_HPP

#include <log4cxx/logger.h>

#include "../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"
#include "../IR/Sentences/SentenceRoot.hpp"
#include "Resolvent.hpp"

namespace optifol
{

class MutableSentenceRoot;
class SymbolRepository;
class Variable;
class IProcessedTerm;

class KnowledgeBase
{
public:
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
        std::unordered_set<Resolvent> resolvents{};
        std::size_t elapsed_step_count = 1;

        QueryResult(const QueryResult&) = delete;
        QueryResult& operator=(const QueryResult&) = delete;

        QueryResult(QueryResult&&) = default;
        QueryResult& operator=(QueryResult&&) = default;
    };

    /**
     * @brief Create a new KnowledgeBase to store Clause objects with reference to the shared SymbolRepository.
     * @param symbol_repository The repository storing symbols used by Clause objects in the KnowledgeBase.
     */
    explicit KnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository);

    /**
     * @brief Attempt to introduce an entire SentenceRoot into the knowledge base.
     * @param sentence The SentenceRoot containing the Clause nodes to introduce.
     * @return Were all Clause objects from the SentenceRoot added?
     */
    bool tell(const SentenceRoot &sentence);

    /**
     * @brief Attempt to introduce a new single Clause into the knowledge base.
     * @param new_clause The new Clause to introduce.
     * @return Was the Clause added?
     */
    bool tell(const Clause &new_clause);

    /**
     * @brief Attempt to determine truth of the given query given the knowledge provided to the KnowledgeBase.
     * @param query The conjecture.
     * @param max_step_count The maximum number of steps taken by the resolution procedure before issuing a time-out.
     * @return The result of the query, including a ConjectureState and execution trace.
     */
    QueryResult ask(std::unique_ptr<MutableSentenceRoot> &&query, size_t max_step_count =
        std::numeric_limits<std::size_t>::max());

private:
    QueryResult run_resolution(size_t max_step_count);

    void collect_unified_literals(const Literal &self, const Clause &source_clause, Clause &destination_clause) const;

    Clause factor_literals(const Clause &unified_clause);

    /**
     * @brief Attempt to introduce an entire SentenceRoot into the knowledge base.
     * @param sentence The SentenceRoot containing the Clause nodes to introduce.
     * @return Were all Clause objects from the SentenceRoot added?
     * @note This should only be used for Clause nodes being introduced mid-way through the resolution process. Use
     *  @ref tell(const SentenceRoot&) for populating the initial knowledge base that persists across queries.
     */
    bool introduce(const SentenceRoot &sentence);

    /**
     * @brief Attempt to introduce a new single Clause into the knowledge base.
     * @param new_clause The new Clause to introduce.
     * @return Was the Clause added?
     * @note This should only be used for Clause nodes being introduced mid-way through the resolution process. Use
     *  @ref tell(const Clause&) for populating the initial knowledge base that persists across queries.
     */
    bool introduce(const Clause& new_clause);

    /**
     * @brief Attempt to insert a new Clause in the given @ref std::unordered_set, providing suitable logging.
     * @param new_clause The incoming Clause to consider adding.
     * @param destination The destination set into which the Clause should be inserted.
     * @return Was the new Clause added?
     */
    static bool insert_clause(const Clause& new_clause, std::unordered_set<Clause>& destination);

    [[nodiscard]] std::vector<Resolvent> find_resolvents(const Clause &lhs_clause, const Clause &rhs_clause);

    static const log4cxx::LoggerPtr kb_logger;
    static const log4cxx::LoggerPtr resolution_logger;
    static const log4cxx::LoggerPtr factoring_logger;

    std::unordered_set<Clause> base_clauses;
    std::unordered_set<Clause> introduced_clauses;
    std::unordered_set<Resolvent> resolution_steps;

    std::shared_ptr<SymbolRepository> symbol_repository;

    UnificationVisitor unifier;
    UnificationVisitor factoring_unifier;

    UnificationApplicationVisitor applicator;
    UnificationApplicationVisitor factoring_applicator;
};

} // namespace optifol

#endif // OPTIFOL_KNOWLEDGEBASE_HPP
