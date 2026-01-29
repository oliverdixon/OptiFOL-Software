/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL binary-unification visitor
 * @author Oliver Dixon
 * @date 2025-06-08
 * @version Development
 */

#ifndef UNIFICATIONVISITOR_H
#define UNIFICATIONVISITOR_H

#include "../../../IR/SymbolRepository.hpp"
#include "UnificationApplicationVisitor.hpp"

namespace optifol
{

class Function;
class ITerm;
class Literal;
class Constant;
class SymbolRepository;

/**
 * @class UnificationVisitor
 * @brief Provides a visitor for binary-unification of FOL terms and sentences
 * @details
 *  <p>
 *      For details of the Unification Problem and particulars of the canonical software implementation, see
 *      <i>Artificial Intelligence, A Modern Approach</i> by Russell and Norvig. In brief, the algorithm considers two
 *      sentences @f$p@f$ and @f$q@f$ and returns a variable substitution map @f$\theta@f$, or <i>unifier</i> if one
 *      exists:
 *      @f[
 *          \text{Unify}(p, q) \mathrel{\vcenter{:}}= \theta \text{ such that }
 *          \text{Sub}(\theta, p) = \text{Sub}(\theta, q).
 *      @f]
 *      For brevity, the definition of our sameness metric is not elaborated here. Clearly, unification is a commutative
 *      operation.
 *  </p>
 *  <p>
 *      The overall association rules within the constraints of the Optifol type systems is:
 *      <ul>
 *          <li>Predicates might unify with predicates only;</li>
 *          <li>Variables might unify with any term;</li>
 *          <li>Constants might unify with constants only;</li>
 *          <li>Functions might unify with functions only; and</li>
 *          <li>Any term might unify with variables only.</li>
 *      </ul>
 *      These rules are reflected in the commutative visitor member functions, and must be respected by unifiable types
 *      implementing the acceptor member functions. Note that triple-despatch may be necessary in some cases (acceptor
 *      calling another overload acceptor, which then calls the visitor) to correctly explore term-specialised cases.
 *  </p>
 *  <p>
 *      Following a unification attempt (<code>visit</code> call), any applicable substitutions will have been traced by
 *      the visitor instance and written to an internal state accessible with @ref observe_substitutions. If unification
 *      was successful, indicated by the return code of <code>visit</code>, the set consists of most-general unifiers.
 *      If unification was unsuccessful, the substitutions will not necessarily produce matching clauses under
 *      application, and the state should be reset with @ref reset_substitutions. This is a conscious API design choice,
 *      as it is occasionally useful for users to inspect the partial substitution trace of a failed unification.
 *  </p>
 * @warning
 *  <p>
 *      There is opportunity for significant optimisation of this procedure. In particular, the implementation
 *      here is reminiscent of Russell and Norvig's adaptation of J. A. Robinson's 1965 exponential algorithm. At some
 *      point, this visitor should use the algorithm of de Champeaux (2022) which is linear-bounded. A Java reference
 *      implementation can be found at https://github.com/ddccc/Unification. See also the paper:
 *      https://doi.org/10.1007/s10817-022-09635-1.
 *  </p>
 * @see UnificationApplicationVisitor for the mutating visitor to apply Unifier elements to ASTs.
 */
class UnificationVisitor
{
public:
    /**
     * @brief Initialise the UnificationVisitor for use with symbols registered in the given SymbolRepository.
     * @param symbol_repository The SymbolRepository containing symbols for the terms received by the visitor.
     */
    explicit UnificationVisitor(std::shared_ptr<SymbolRepository> symbol_repository);

    /**
     * @brief Attempt to unify two predicates (Literal) on the predicate and component-wise arguments.
     * @details Unification between two predicates will succeed if and only if they are identically named, have the same
     *  number of term arguments, and the terms can be zipped and mutually unified i.a.w. other rules described herein.
     * @param predicate_lhs The LHS Literal to unify
     * @param predicate_rhs The RHS Literal to unify
     * @return Can the LHS and RHS Literal objects be unified?
     */
    [[nodiscard]] bool visit(const Literal &predicate_lhs, const Literal &predicate_rhs);

    /**
     * @brief Attempt to unify two Constant terms
     * @details Unification between two Constant terms will succeed if and only if they are identically named.
     * @param constant_lhs The LHS Constant to unify
     * @param constant_rhs The RHS Constant to unify
     * @return Can the LHS and RHS Constant objects be unified?
     */
    [[nodiscard]] static bool visit(const Constant &constant_lhs, const Constant &constant_rhs);

    /**
     * @brief Attempt to unify a Variable and a Constant term.
     * @details Unification between a Variable and a Constant will succeed if and only if they are identically named and
     *  substitution does not result in an infinite cycle (the "occurs check" condition).
     * @param variable_lhs The LHS Variable to unify
     * @param constant_rhs The RHS Constant to unify
     * @return Can the LHS and RHS objects be unified?
     */
    [[nodiscard]] bool visit(const Variable &variable_lhs, const Constant &constant_rhs);

    /**
     * @brief Attempt to unify a Variable and a Function term.
     * @details Unification between a Variable and Function will succeed according to the same conditions as those
     *  required by @ref visit(const Variable&, const Constant&).
     * @param variable_lhs The LHS Variable to unify
     * @param function_rhs The RHS Function to unify
     * @return Can the LHS and RHS objects be unified?
     */
    [[nodiscard]] bool visit(const Variable &variable_lhs, const Function &function_rhs);

    /**
     * @brief Attempt to unify two Variable terms.
     * @details Unification between two Variable terms will succeed according to the same conditions as those required
     *  by @ref visit(const Variable&, const Constant&).
     * @param variable_lhs The LHS Variable to unify
     * @param variable_rhs The RHS Variable to unify
     * @return Can the LHS and RHS Variable objects be unified?
     */
    [[nodiscard]] bool visit(const Variable &variable_lhs, const Variable &variable_rhs);

    /**
     * @brief Attempt to unify two Function terms.
     * @details Unification between two Function terms will succeed according to similar conditions as those required
     *  by @ref visit(const Literal&, const Literal&), i.e. identically named symbols and component-wise unification.
     * @param function_lhs The LHS Function to unify
     * @param function_rhs The RHS Function to unify
     * @return Can the LHS and RHS Function objects be unified?
     */
    [[nodiscard]] bool visit(const Function &function_lhs, const Function &function_rhs);

    /**
     * @brief Observe the working set of substitutions produced since the last @ref reset_substitutions call.
     *
     * @details The Unifier set consists of substitutions explored by the visitor whilst attempting to force two clauses
     *  to be equivalent under application. If a <code>visit</code> call indicated successful unification, the
     *  substitutions represent the most-general unifiers required to make the two respective clauses equivalent under
     *  the UnificationApplicationVisitor. An empty set, or set returned following a failed unification attempt, may be
     *  useful to callers but are not unifiers.
     *
     * @return The working Unifier set.
     */
    [[nodiscard]] const Unifier *observe_substitutions() const noexcept;

    [[nodiscard]] std::shared_ptr<Unifier> share_substitutions() const noexcept;

    /**
     * @brief Clear unifying substitutions and reset the state such that @ref observe_substitutions produces an empty
     *  working set.
     */
    void reset_substitutions() const noexcept;

private:
    /**
     * @brief Attempt to unify a variable with a non-variable ("generic") term.
     * @details
     *      To unify a variable with a generic term, they must be one of the following. Providing that unification is
     *      successful in the non-trivial sense, a substitution is added to the map.
     *      <ul>
     *          <li>Hash-identical: if they have the same hash, they are assumed to refer the same object. Unification
     *              is valid in the trivial sense, and an explicit substitution does not need to be recorded.</li>
     *          <li>Not chain-ununifiable: if the variable is already bound to a substitution, the bound term must be
     *              unifiable to the generic term.</li>
     *      </ul>
     * @param variable_lhs The LHS variable
     * @param generic_term_rhs The RHS generic term
     * @return Can the variable and term be unified?
     */
    [[nodiscard]] bool variable_generic(const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs);

    /**
     * @brief Registers a new substitution @f$ \alpha \mapsto \beta @f$ for a Variable @f$ \alpha @f$ and generic
     *  IProcessedTerm @f$ \beta @f$ in terms of their existing pointers in the @ref symbol_repository.
     * @param bound_key The @f$ \alpha @f$ Variable key to bind.
     * @param bound_value The @f$ \beta @f$ IProcessedTerm binding.
     * @throws SemanticException if either @f$ \alpha @f$ or @f$ \beta @f$ do not exist in the @ref symbol_repository.
     */
    void register_substitution(const Variable &bound_key, const IProcessedTerm &bound_value) const;

    /**
     * @brief Determines whether the LHS Variable occurs in the RHS IProcessedTerm, or any applicable substitutions
     *  thereof.
     * @details
     *  <p>
     *      A failure of the 'occurs check' procedure informs a unifier whether the introduction of a Variable
     *      substitution will cause a cycle with itself, or with an existing substitution in the unifier @f$ \theta @f$.
     *  </p>
     *  <p>
     *      If the presently generated unifier @f$ \theta = \left\{ \theta_1, \ldots, \theta_i \right\} @f$ is already
     *      composed of @f$ i @f$ substitutions, and the occurs checker is considering the addition of a new substitution
     *      @f$ \theta_{i+1} = \left[ \alpha \mapsto \beta \right] @f$, <code>true</code> is returned if and only if
     *      @f$ \alpha @f$ appears in the expansion of @f$ \beta @f$ or any of the following:
     *      @f[
     *          \text{Sub}\left( \theta_1, \beta \right), \ldots, \text{Sub}\left( \theta_i, \beta \right).
     *      @f]
     *  </p>
     * @param variable_lhs The Variable for which to search in the substituted enumeration of the RHS IProcessedTerm.
     * @param generic_term_rhs The generic IProcessedTerm to explore under substitutions, searching for the Variable.
     * @return Does the Variable appear in the IProcessedTerm, or any isomorphisms (under substitution) using the
     *  presently generated substitutions?
     */
    [[nodiscard]] bool occurs_check(const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs) const;

    /**
     * @brief The SymbolRepository for the environment of the unified pair, provided by the consumer.
     */
    const std::shared_ptr<SymbolRepository> symbol_repository;

    /**
     * @brief The working set of Variable-to-Term substitutions for the unification attempt. Once unification has
     *  returned a verdict, the final set can be observed with @ref observe_substitutions.
     */
    const std::shared_ptr<Unifier> substitutions;

    /**
     * @brief A helper substitution applicator for @ref occurs_check.
     * @note This is <code>mutable</code> because it's essentially a throw-away, single-use cache for the occurs check,
     *  and we don't want to break API semantics by indicating the occurs-check procedure is non-constant on the visitor
     *  object.
     */
    mutable UnificationApplicationVisitor application_visitor;
};

}

#endif
