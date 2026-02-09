/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL resolution prover
 * @author Oliver Dixon
 * @date 2025-09-14
 * @version Development
 */

#ifndef OPTIFOL_PROVER_HPP
#define OPTIFOL_PROVER_HPP

#include <log4cxx/logger.h>
#include <ranges>

#include "../IR/Sentences/Clause.hpp"
#include "../Visitors/RegularTargets/Unification/BidirectionalUnificationVisitor.hpp"
#include "FVIKnowledgeBase.hpp"

namespace optifol
{

class MutableSentenceRoot;
class SymbolRepository;
class Variable;
class IProcessedTerm;

class Resolvent;
struct QueryResult;

/**
 * @class Prover
 * @brief A Prover manages the storage of CNF-normalised Clause objects and allows users to query the validity of
 *  statements under the specified model.
 * @details
 *  <p>
 *      The typical workflow is straightforward:
 *      <ol>
 *          <li>The user creates a new Prover instance linked to a persistent SymbolRepository;</li>
 *          <li>The user issues @ref tell commands to incrementally build the axioms of the Prover's contextual
 *              knowledge base;</li>
 *          <li>Once the Prover has been informed of all the axioms, the user issues an @ref ask command with a query to
 *              attempt to derive a contradiction or model proof attesting to the consistency of the given conjecture
 *              under the axioms.</li>
 *          <li>After the proof has completed, the user browses the execution metadata and proof trace (organised as
 *              a binary tree between Clauses and Resolvents). This can be directly visualised.</li>
 *      </ol>
 *  </p>
 *  <p>
 *      Users should be careful with the data-ownership semantics of the Prover. QueryResults may observe any items in
 *      the SymbolRepository, and also any base clauses (issued with @ref tell) maintained by the Prover. Therefore, the
 *      Prover should outlive the SymbolRepository, which should in turn outlive the Prover. This model is required due
 *      to the performance implications of copying Clauses and Resolvents.
 *  </p>
 */
class Prover
{
public:
    /**
     * @brief Create a new Prover instance to store Clause objects with reference to the shared SymbolRepository.
     * @param symbol_repository The repository storing symbols used by Clause objects in the Prover.
     */
    explicit Prover(std::shared_ptr<SymbolRepository> symbol_repository);

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
     * @brief Attempt to determine truth of the given query given the knowledge provided to the Prover.
     * @param query The conjecture.
     * @param max_step_count The maximum number of steps taken by the resolution procedure before issuing a time-out.
     * @return The result of the query, including a ConjectureState and execution trace.
     * @note Take note of the Prover class documentation on data lifetimes before making use of the QueryResult.
     */
    QueryResult ask(std::unique_ptr<MutableSentenceRoot> &&query, size_t max_step_count =
        std::numeric_limits<std::size_t>::max());

    Prover(Prover&&) = default;

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
        static bool operator()(const std::unique_ptr<Resolvent>& lhs, const std::unique_ptr<Resolvent>& rhs) noexcept;
    };

    /**
     * @brief Drive and organise the binary resolution procedure on the populated knowledge base, attempting to derive a
     *  contradiction.
     * @param negated_query A CNF-normalised construction of the negation of the goal.
     * @param max_step_count The maximum number of steps to perform in the resolution procedure before bailing out.
     * @return Execution metadata and a proof trace.
     * @invariant Any Clauses referenced by Resolvents must be present in one of the standard FVIKnowledgeBase
     *  locations:
     *  <ul>
     *      <li>The <i>base clauses</i> store of the Prover; or</li>
     *      <li>The <i>introduced clauses</i> store in QueryResult.</li>
     *  </ul>
     */
    QueryResult run_resolution(std::unique_ptr<SentenceRoot> &&negated_query, size_t max_step_count);

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
     * @param lhs_node The source LHS node containing Literals to subject to binary resolution.
     * @param rhs_node The source RHS node containing Literals to subject to binary resolution.
     * @return The generated Resolvents, and owning containers of resolutions referenced by the Resolvents.
     */
    [[nodiscard]] std::vector<std::pair<Resolvent, std::unique_ptr<Clause>>> find_resolvents(
            const ProofTreeNode *lhs_node, const ProofTreeNode *rhs_node);

    static const log4cxx::LoggerPtr prover_logger;
    static const log4cxx::LoggerPtr resolution_logger;
    static const log4cxx::LoggerPtr factoring_logger;

    std::shared_ptr<SymbolRepository> symbol_repository;
    FVIKnowledgeBase base_clauses;

    BidirectionalUnificationVisitor unifier;
    BidirectionalUnificationVisitor factoring_unifier;

    UnificationApplicationVisitor applicator;
    UnificationApplicationVisitor factoring_applicator;
};

} // namespace optifol

#endif // OPTIFOL_PROVER_HPP
