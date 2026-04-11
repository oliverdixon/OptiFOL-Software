/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL binary-unification visitor (unidirectional)
 * @author Oliver Dixon
 * @date 2025-06-08
 * @version Development
 */

#ifndef UNIFICATIONVISITOR_H
#define UNIFICATIONVISITOR_H

#include "../IObservingBinaryVisitor.hpp"
#include "UnificationApplicationVisitor.hpp"

namespace optifol
{

class ITerm;
class SymbolRepository;

/**
 * @class UnificationVisitor
 * @brief Provides a visitor for binary-unification of FOL terms and sentences
 * @details
 *  <p>
 *      For details of the Unification Problem and particulars of the canonical software implementation, see
 *      <i>Artificial Intelligence, A Modern Approach</i> by Russell and Norvig. In brief, the algorithm
 * considers two sentences @f$p@f$ and @f$q@f$ and returns a variable substitution map @f$\theta@f$, or
 * <i>unifier</i> if one exists:
 *      @f[
 *          \text{Unify}(p, q) \mathrel{\vcenter{:}}= \theta \text{ such that }
 *          \text{Sub}(\theta, p) = \text{Sub}(\theta, q).
 *      @f]
 *      For brevity, the definition of our sameness metric is not elaborated here.
 *  </p>
 *  <p>
 *      The most basic UnificationVisitor is unidirectional; that is, it accepts a "generalisation" and an
 * "instance", and substitutions may only be made in the generalisation. If substitutions should also be made
 * in the instance, use the BidirectionalUnificationVisitor instead, which includes an occurs-check to verify
 * correctness of substitutions.
 *  </p>
 *  <p>
 *      The overall association rules within the constraints of the Optifol type systems is:
 *      <ul>
 *          <li>Predicates might unify with predicates only;</li>
 *          <li>Variables might unify with any term;</li>
 *          <li>Functions might unify with functions only; and</li>
 *          <li>Any term might unify with variables only.</li>
 *      </ul>
 *      These rules are reflected in the visitor member functions, and must be respected by unifiable types
 * implementing the acceptor member functions. Note that triple-despatch may be necessary in some cases
 * (acceptor calling another overload acceptor, which then calls the visitor) to correctly explore
 * term-specialised cases.
 *  </p>
 *  <p>
 *      Following a unification attempt (<code>visit</code> call), any applicable substitutions will have been
 * traced by the visitor instance and written to an internal state accessible with @ref observe_substitutions.
 * If unification was successful, indicated by the return code of <code>visit</code>, the set consists of
 * most-general unifiers. If unification was unsuccessful, the substitutions will not necessarily produce
 * matching clauses under application, and the state should be reset with @ref reset_substitutions. This is a
 * conscious API design choice, as it is occasionally useful for users to inspect the partial substitution
 * trace of a failed unification.
 *  </p>
 * @note
 *  <p>
 *      There is opportunity for significant optimisation of this procedure. In particular, the implementation
 *      here is reminiscent of Russell and Norvig's adaptation of J. A. Robinson's 1965 exponential algorithm.
 * At some point, this visitor should use the algorithm of de Champeaux (2022) which is linear-bounded. A Java
 * reference implementation can be found at https://github.com/ddccc/Unification. See also the paper:
 *      https://doi.org/10.1007/s10817-022-09635-1.
 *  </p>
 * @see BidirectionalUnificationVisitor for the unification visitor to produce substitutions on both the
 * generalisation and the instance.
 * @see UnificationApplicationVisitor for the mutating visitor to apply Unifier elements to ASTs.
 */
class UnificationVisitor : public IObservingBinaryVisitor
{
public:
    /**
     * @brief Initialise the UnificationVisitor for use with symbols registered in the given SymbolRepository.
     * @param symbol_repository The SymbolRepository containing symbols for the terms received by the visitor.
     */
    explicit UnificationVisitor(std::shared_ptr<SymbolRepository> symbol_repository);

    /**
     * @brief Attempt to unify two predicates (Literals) on the predicate and component-wise arguments.
     * @details Unification between two predicates will succeed if and only if they are identically named,
     * have the same number of term arguments, have the same polarity, and the terms can be zipped and
     * mutually unified i.a.w. other rules described herein.
     * @param predicate_gen The generalisation Literal to unify
     * @param predicate_inst The instance Literal to unify
     * @return Can the generalisation be unified into the instance?
     */
    [[nodiscard]] bool visit(const Literal &predicate_gen, const Literal &predicate_inst) override;

    /**
     * @brief Attempt to unify a Variable and a Function term.
     * @param variable_gen The LHS Variable to unify
     * @param function_inst The RHS Function to unify
     * @return Can the LHS and RHS objects be unified?
     */
    [[nodiscard]] bool visit(const Variable &variable_gen, const Function &function_inst) override;

    /**
     * @brief Attempt to unify two Variable terms.
     * @details Unification between two Variable terms will succeed according to the same conditions as those
     * required by @ref visit(const Variable&, const Constant&).
     * @param variable_gen The LHS Variable to unify
     * @param variable_inst The RHS Variable to unify
     * @return Can the LHS and RHS Variable objects be unified?
     */
    [[nodiscard]] bool visit(const Variable &variable_gen, const Variable &variable_inst) override;

    /**
     * @brief Attempt to unify a Function and a Variable.
     * @param function_gen The LHS Function to unify
     * @param variable_inst The RHS Variable to unify
     * @return False, since a Function and a Variable could only be unified if the Variable could be
     * substituted. In this case, it's in the instance.
     */
    [[nodiscard]] bool visit(const Function &function_gen, const Variable &variable_inst) override;

    /**
     * @brief Attempt to unify two Function terms.
     * @details Unification between two Function terms will succeed according to similar conditions as those
     * required by @ref visit(const Literal&, const Literal&), i.e. identically named symbols and
     * component-wise unification.
     * @param function_gen The LHS Function to unify
     * @param function_inst The RHS Function to unify
     * @return Can the LHS and RHS Function objects be unified?
     */
    [[nodiscard]] bool visit(const Function &function_gen, const Function &function_inst) override;

    /**
     * @brief Observe the working set of substitutions produced since the last @ref reset_substitutions call.
     *
     * @details The Unifier set consists of substitutions explored by the visitor whilst attempting to force
     * two clauses to be equivalent under application. If a <code>visit</code> call indicated successful
     * unification, the substitutions represent the most-general unifiers required to make the two respective
     * clauses equivalent under the UnificationApplicationVisitor. An empty set, or set returned following a
     * failed unification attempt, may be useful to callers but are not unifiers.
     *
     * @return The working Unifier set.
     */
    [[nodiscard]] const Unifier *observe_substitutions() const noexcept;

    [[nodiscard]] std::shared_ptr<Unifier> share_substitutions() const noexcept;

    /**
     * @brief Clear unifying substitutions and reset the state such that @ref observe_substitutions produces
     * an empty working set.
     */
    void reset_substitutions() const noexcept;

protected:
    /**
     * @brief Attempt to unify a variable with a generic term.
     * @details To unify a variable with a generic term in the unidirectional case, the mapping must either be
     * new, or be identical to an existing entry.
     * @param variable_gen The LHS variable
     * @param generic_term_inst The RHS generic term
     * @return Can the variable and term be unified?
     */
    [[nodiscard]] virtual bool variable_generic(
            const Variable &variable_gen, const IProcessedTerm &generic_term_inst);

    /**
     * @brief Registers a new substitution @f$ \alpha \mapsto \beta @f$ for a Variable @f$ \alpha @f$ and
     * generic IProcessedTerm @f$ \beta @f$ in terms of their existing pointers in the @ref symbol_repository.
     * @param bound_key The @f$ \alpha @f$ Variable key to bind.
     * @param bound_value The @f$ \beta @f$ IProcessedTerm binding.
     * @throws SemanticException if either @f$ \alpha @f$ or @f$ \beta @f$ do not exist in the @ref
     * symbol_repository.
     */
    void register_substitution(const Variable &bound_key, const IProcessedTerm &bound_value) const;

    /**
     * @brief The SymbolRepository for the environment of the unified pair, provided by the consumer.
     */
    const std::shared_ptr<SymbolRepository> symbol_repository;

    /**
     * @brief The working set of Variable-to-Term substitutions for the unification attempt. Once unification
     * has returned a verdict, the final set can be observed with @ref observe_substitutions.
     */
    const std::shared_ptr<Unifier> substitutions;
};

} // namespace optifol

#endif
