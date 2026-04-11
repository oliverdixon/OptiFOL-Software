/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL binary-unification visitor (bidirectional)
 * @author Oliver Dixon
 * @date 2025-06-08
 * @version Development
 */

#ifndef BIDIRECTIONALUNIFICATIONVISITOR_H
#define BIDIRECTIONALUNIFICATIONVISITOR_H

#include "UnificationApplicationVisitor.hpp"
#include "UnificationVisitor.hpp"

namespace optifol
{

class ITerm;
class SymbolRepository;

/**
 * @class BidirectionalUnificationVisitor
 * @brief Bidirectional variant of the UnificationVisitor, permitting substitutions in the generalisation and
 * the instance, and includes an occurs-check for sanity-checking substitutions.
 */
class BidirectionalUnificationVisitor : public UnificationVisitor
{
public:
    /**
     * @brief Initialise the BidirectionalUnificationVisitor for use with symbols registered in the given
     *  SymbolRepository.
     * @param symbol_repository The SymbolRepository containing symbols for the terms received by the visitor.
     */
    explicit BidirectionalUnificationVisitor(std::shared_ptr<SymbolRepository> symbol_repository);

    [[nodiscard]] bool visit(const Function &function_lhs, const Variable &variable_rhs) override;

protected:
    /**
     * @brief Attempt to unify a variable with a non-variable ("generic") term.
     * @details
     *      To unify a variable with a generic term, they must be one of the following. Providing that
     * unification is successful in the non-trivial sense, a substitution is added to the map. <ul>
     *          <li>Hash-identical: if they have the same hash, they are assumed to refer the same object.
     * Unification is valid in the trivial sense, and an explicit substitution does not need to be
     * recorded.</li> <li>Not chain-ununifiable: if the variable is already bound to a substitution, the bound
     * term must be unifiable to the generic term.</li>
     *      </ul>
     * @param variable_lhs The LHS variable
     * @param generic_term_rhs The RHS generic term
     * @return Can the variable and term be unified?
     */
    [[nodiscard]] bool variable_generic(
            const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs) override;

private:
    /**
     * @brief Determines whether the LHS Variable occurs in the RHS IProcessedTerm, or any applicable
     * substitutions thereof.
     * @details
     *  <p>
     *      A failure of the 'occurs check' procedure informs a unifier whether the introduction of a Variable
     *      substitution will cause a cycle with itself, or with an existing substitution in the unifier @f$
     * \theta @f$.
     *  </p>
     *  <p>
     *      If the presently generated unifier @f$ \theta = \left\{ \theta_1, \ldots, \theta_i \right\} @f$ is
     * already composed of @f$ i @f$ substitutions, and the occurs checker is considering the addition of a
     * new substitution
     *      @f$ \theta_{i+1} = \left[ \alpha \mapsto \beta \right] @f$, <code>true</code> is returned if and
     * only if
     *      @f$ \alpha @f$ appears in the expansion of @f$ \beta @f$ or any of the following:
     *      @f[
     *          \text{Sub}\left( \theta_1, \beta \right), \ldots, \text{Sub}\left( \theta_i, \beta \right).
     *      @f]
     *  </p>
     * @param variable_lhs The Variable for which to search in the substituted enumeration of the RHS
     * IProcessedTerm.
     * @param generic_term_rhs The generic IProcessedTerm to explore under substitutions, searching for the
     * Variable.
     * @return Does the Variable appear in the IProcessedTerm, or any isomorphisms (under substitution) using
     * the presently generated substitutions?
     */
    [[nodiscard]] bool occurs_check(const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs);

    /**
     * @brief A helper substitution applicator for @ref occurs_check.
     * @note This is <code>mutable</code> because it's essentially a throw-away, single-use cache for the
     * occurs check, and we don't want to break API semantics by indicating the occurs-check procedure is
     * non-constant on the visitor object.
     */
    UnificationApplicationVisitor application_visitor;
};

} // namespace optifol

#endif
