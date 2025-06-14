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

#include <optional>

#include "../../../IR/Substitution.hpp"

namespace optifol
{

class Function;
class ITerm;
class Variable;
class Predicate;

/**
 * @class UnificationVisitor
 * @brief Provides a visitor for binary-unification of FOL terms and sentences
 * @details
 *  <p>
 *      For details of the Unification Problem and particulars of the canonical software implementation, see
 *      <i>Artificial Intelligence, A Modern Approach</i> by Russell and Norvig. In brief, the algorithm considers two
 *      sentences \f$p\f$ and \f$q\f$ and returns a variable substitution map \f$\theta\f$, or <i>unifier</i> if one
 *      exists:
 *      \f[
 *          \text{Unify}(p, q) \mathrel{\vcenter{:}}= \theta \text{ such that }
 *          \text{Sub}(\theta, p) = \text{Sub}(\theta, q).
 *      \f]
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
public:

    /**
     * @brief Attempt to unify two predicates
     * @details To unify two predicates, they must have matching names, argument vectors of equal lengths, and
     *  pairwise-unifiable arguments.
     * @param predicate_lhs The LHS predicate
     * @param predicate_rhs The RHS predicate
     * @return Can the predicates be unified?
     */
    [[nodiscard]] bool visit(const Predicate &predicate_lhs, const Predicate &predicate_rhs);

    /**
     * @brief Attempt to unify a variable with a non-variable generic term
     * @details
     *  <p>
     *      To unify a variable with a generic term, they must be one of the following:
     *      <ul>
     *          <li>Hash-identical: if they have the same hash, they are assumed to refer the same object. Unification
     *              is valid in the trivial sense, and an explicit substitution does not need to be recorded.</li>
     *          <li>Not chain-ununifiable: if the variable is already bound to a substitution, the bound term must be
     *              unifiable to the generic term.</li>
     *      </ul>
     *  </p>
     *  <p>
     *      Providing that unification is successful in the non-trivial sense, a substitution is added to the map.
     *  </p>
     * @param variable_lhs The LHS variable
     * @param generic_term_rhs The RHS generic term
     * @return Can the variable and term be unified?
     * @todo Add occurs check to avoid cycles in the substitution map.
     */
    [[nodiscard]] bool visit(const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs);

    /**
     * @brief Attempt to unify two variables
     * @details To unify two variables, consider the process for unifying a variable and non-variable term. The process
     *  is identical, except for an additional check to ensure that the other (RHS) variable is not chain-ununifiable.
     *  See UnificationVisitor::visit(const MutableVariable &, IMutableTerm &).
     * @param variable_lhs The LHS variable
     * @param variable_rhs The RHS variable
     * @return Can the variables be unified?
     * @todo Add occurs check to avoid cycles in the substitution map.
     */
    [[nodiscard]] bool visit(const Variable &variable_lhs, const Variable &variable_rhs);

    /**
     * @brief Attempt to unify two functions
     * @details To unify two functions, refer to the process for unifying two predicates defined in
     *  UnificationVisitor::visit(const MutablePredicate&, const MutablePredicate&); it is isomorphic from predicates
     *  (sentence instantiations) to functions (term instantiations).
     * @param function_lhs The LHS function
     * @param function_rhs The RHS function
     * @return Can the functions be unified?
     */
    [[nodiscard]] bool visit(const Function &function_lhs, const Function &function_rhs);

    /**
     * @brief Constant base-case to catch attempts to unify a generic term with a function
     * @param generic_term_lhs The LHS generic term
     * @param function_rhs The RHS generic function
     * @return Always false; these types are not eligible unification candidates.
     */
    [[nodiscard]] static bool visit(const IProcessedTerm &generic_term_lhs, const Function &function_rhs);

    /**
     * @brief Constant base-case to catch attempts to unify non-specialised generic terms
     * @param generic_term_lhs The LHS generic term
     * @param generic_term_rhs The RHS generic term
     * @return Always false; these types are not eligible unification candidates.
     */
    [[nodiscard]] static bool visit(const IProcessedTerm &generic_term_lhs, const IProcessedTerm &generic_term_rhs);

    const std::optional<Substitution> &observe_substitutions() const;

private:
    void register_substitution(const Variable &bound_key, const IProcessedTerm &bound_value);

    std::optional<Substitution> substitutions;
};

}

#endif
