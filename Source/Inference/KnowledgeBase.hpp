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
#include <ranges>

#include "../IR/Sentences/SentenceRoot.hpp"
#include "../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"
#include "QueryResult.hpp"
#include "Resolvent.hpp"

namespace optifol
{

class MutableSentenceRoot;
class SymbolRepository;
class Variable;
class IProcessedTerm;

/**
 * @class KnowledgeBase
 * @brief A KnowledgeBase stores CNF-normalised Clause objects and allows users to query the validity of statements
 *  under the specified model.
 * @details
 *  <p>
 *      The typical workflow is straightforward:
 *      <ol>
 *          <li>The user creates a new KnowledgeBase linked to a persistent SymbolRepository;</li>
 *          <li>The user issues @ref tell commands to incrementally build the axioms of the KnowledgeBase;</li>
 *          <li>Once the KnowledgeBase has been informed of all the axioms, the user issues a @ref ask command with
 *              a query to attempt to derive a contradiction or model proof attesting to the consistency of the given
 *              conjecture under the axioms.</li>
 *          <li>After the proof has completed, the user browses the execution metadata and proof trace (organised as
 *              a binary tree between Clauses and Resolvents). This can be directly visualised.</li>
 *      </ol>
 *  </p>
 *  <p>
 *      Users should be careful with the data-ownership semantics of the KnowledgeBase. QueryResults may observe any
 *      items in the SymbolRepository, and also any base clauses (issued with @ref tell) maintained by the
 *      KnowledgeBase. Therefore, the KnowledgeBase should outlive the SymbolRepository, which should in turn outlive
 *      the KnowledgeBase. This model is required due to the performance implications of copying Clauses and Resolvents.
 *  </p>
 */
class KnowledgeBase
{
public:
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
     * @note Take note of the KnowledgeBase class documentation on data lifetimes before making use of the QueryResult.
     */
    QueryResult ask(std::unique_ptr<MutableSentenceRoot> &&query, size_t max_step_count =
        std::numeric_limits<std::size_t>::max());

    KnowledgeBase(KnowledgeBase&&) = default;

private:
    /**
     * @struct PQResolventUnitPref
     * @brief Helper comparator for imposing the unit-preference strict weak ordering on owned Resolvent objects.
     */
    struct PQResolventUnitPref
    {
        /**
         * @brief Is the Resolvent owned by the LHS more preferable than the Resolvent owned by the RHS?
         * @param lhs LHS Resolvent owning container
         * @param rhs RHS Resolvent owning container
         * @return LHS-RHS relation
         */
        static bool operator()(const std::unique_ptr<Resolvent>& lhs, const std::unique_ptr<Resolvent>& rhs) noexcept
        {
            return *lhs < *rhs;
        }
    };

    /**
     * @brief Drive and organise the binary resolution procedure on the populated knowledge base, attempting to derive a
     *  contradiction.
     * @param max_step_count The maximum number of steps to perform in the resolution procedure before bailing out.
     * @return Execution metadata and a proof trace.
     * @invariant Any Clauses referenced by Resolvents must be present in one of the standard locations:
     *  <ul>
     *      <li>The <i>base clause</i> store of the KnowledgeBase;</li>
     *      <li>The <i>introduced clause</i> store of the KnowledgeBase; or</li>
     *      <li>The managed clause store in QueryResult.</li>
     *  </ul>
     *  (This is enforced at runtime.)
     */
    QueryResult run_resolution(size_t max_step_count);

    /**
     * @brief Apply the state UnificationApplicationVisitor to the given source Clause, filtering the given Literal, and
     *  add the results to the given destination Clause.
     * @param self The Literal contained within the source Clause to filter out.
     * @param source_clause The Clause containing Literal objects to be subject to the applicator.
     * @param destination_clause The Clause to receive the filtered and transformed Literal objects from the source.
     */
    std::unique_ptr<Clause> collect_unified_literals(
            const Literal &self, const Clause &source_clause, std::unique_ptr<Clause> &&destination_clause) const;

    /**
     * @brief Attempt to simplify the given Clause, ideally to a tautology, using the factoring unifier.
     * @param unified_clause The owning container of the target Clause.
     * @return The owning container of the maximally simplified (under unifier application) Clause.
     */
    std::unique_ptr<Clause> factor_literals(std::unique_ptr<Clause> &&unified_clause);

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
    bool introduce(std::unique_ptr<Clause> &&new_clause);

    /**
     * @brief Attempt to insert a new Clause in the given @ref std::unordered_set, providing suitable logging.
     * @param new_clause The incoming Clause to consider adding.
     * @param destination The destination set into which the Clause should be inserted.
     * @return Was the new Clause added?
     */
    static bool insert_clause(std::unique_ptr<Clause> &&new_clause, UniqueUnorderedSet<Clause> &destination);

    /**
     * @brief Perform binary resolution on a pair of Clauses.
     * @details Over the Cartesian product of Literals in the given Clauses, we search for and produce Resolvents as
     *  follows:
     *  <ol>
     *      <li>Attempt to unify the LHS Literal with the negation of the RHS Literal;</li>
     *      <li>If a (most general) unifier is found, collect the set of Literals (excluding the two source objects)
     *          under the application of the MGU;</li>
     *      <li>Attempt to derive a tautology through factoring; see @ref factor_literals;</li>
     *      <li>If a tautology cannot be derived, construct a Resolvent containing the clause as its resolution, also
     *          keeping track of the MGU and source Clauses.</li>
     *  </ol>
     * @param lhs_clause The source LHS Clause containing Literals to subject to binary resolution.
     * @param rhs_clause The source RHS Clause containing Literals to subject to binary resolution.
     * @return The generated Resolvents, and owning containers of resolutions referenced by the Resolvents.
     */
    [[nodiscard]] std::vector<std::pair<Resolvent, std::unique_ptr<Clause>>> find_resolvents(
            const Clause *lhs_clause, const Clause *rhs_clause);

    static constexpr auto unwrap_clause = std::views::transform(
        [](const std::unique_ptr<Clause>& clause){ return clause.get(); });

    static const log4cxx::LoggerPtr kb_logger;
    static const log4cxx::LoggerPtr resolution_logger;
    static const log4cxx::LoggerPtr factoring_logger;

    UniqueUnorderedSet<Clause> base_clauses;
    UniqueUnorderedSet<Clause> introduced_clauses;

    std::shared_ptr<SymbolRepository> symbol_repository;

    UnificationVisitor unifier;
    UnificationVisitor factoring_unifier;

    UnificationApplicationVisitor applicator;
    UnificationApplicationVisitor factoring_applicator;
};

} // namespace optifol

#endif // OPTIFOL_KNOWLEDGEBASE_HPP
