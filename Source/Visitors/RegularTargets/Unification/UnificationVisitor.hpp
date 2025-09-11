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

#include <unordered_map>

#include "../../../DereferencingEqualityFunctor.hpp"
#include "../../../IR/SymbolRepository.hpp"
#include "../../../IR/Terms/Variable.hpp"

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
 *      For brevity, the definition of our sameness metric is not elaborated here.
 *  </p>
 *  <p>
 *      The overall association rules within the constraints of the Optifol type systems is:
 *      <ul>
 *          <li>Predicates might unify with predicates only;</li>
 *          <li>Variables might unify with any term;</li>
 *          <li>Functions might unify with functions only; and</li>
 *          <li>Any term might unify with variables only.</li>
 *      </ul>
 *      These rules are reflected in the commutative visitor member functions, and must be respected by unifiable types
 *      implementing the acceptor member functions. Note that triple-despatch may be necessary in some cases (acceptor
 *      calling another overload acceptor, which then calls the visitor) to correctly explore term-specialised cases.
 *  </p>
 * @warning
 *  <p>
 *      There is opportunity for significant optimisation of this procedure. In particular, the implementation
 *      here is reminiscent of Russell and Norvig's adaptation of J. A. Robinson's 1965 exponential algorithm. At some
 *      point, this visitor should use the algorithm of de Champeaux (2022) which is linear-bounded. A Java reference
 *      implementation can be found at https://github.com/ddccc/Unification. See also the paper:
 *      https://doi.org/10.1007/s10817-022-09635-1.
 *  </p>
 */
class UnificationVisitor
{
    std::unordered_map<const Variable *, const IProcessedTerm *, std::hash<Variable>,
        DereferencingEqualityFunctor<const Variable *, const Variable>> substitutions;

public:
    using Substitution = std::optional<std::pair<const IProcessedTerm *, const IProcessedTerm *>>;
    using SubstitutionMap = decltype(substitutions);

    explicit UnificationVisitor(std::shared_ptr<SymbolRepository> symbol_repository);

    [[nodiscard]] bool visit(const Literal &predicate_lhs, const Literal &predicate_rhs);

    [[nodiscard]] bool visit(const Variable &variable_lhs, const Constant &constant_rhs);

    [[nodiscard]] bool visit(const Variable &variable_lhs, const Function &function_rhs);

    [[nodiscard]] bool visit(const Variable &variable_lhs, const Variable &variable_rhs);

    [[nodiscard]] bool visit(const Function &function_lhs, const Function &function_rhs);

    decltype(substitutions)::const_iterator get_substitutions_cbegin() const noexcept;

    decltype(substitutions)::const_iterator get_substitutions_cend() const noexcept;

private:
    /**
     * @brief Attempt to unify a variable with a non-variable/"generic" term.
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
    void register_substitution(const Variable &bound_key, const IProcessedTerm &bound_value);

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
     * @brief A dummy SymbolRepository for tracking symbols introduced by the UnificationApplicationVisitor during
     *  @ref occurs_check temporary substitution.
     */
    const std::shared_ptr<SymbolRepository> occurs_dummy_symbol_repo = std::make_shared<SymbolRepository>();

    /**
     * @brief The SymbolRepository for the environment of the unified pair, provided by the consumer.
     */
    const std::shared_ptr<SymbolRepository> symbol_repository;
};

}

#endif
