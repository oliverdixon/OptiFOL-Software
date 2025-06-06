/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Unifying Candidate base class
 * @author Oliver Dixon
 * @date 2025-06-07
 * @version Development
 */

#ifndef UNIFYCANDIDATEBASE_HPP
#define UNIFYCANDIDATEBASE_HPP

#include <optional>

#include "Substitution.hpp"

namespace optifol
{

class FunctionNode;
class PredicationNode;
class ITermNode;

/**
 * @class UnifyCandidateBase
 * @brief Unification candidates are eligible for attempted unification with a fixed set of types, typically other
 *  unification candidates.
 * @details For details of the Unification Problem and particulars of the canonical software implementation, see
 *  <i>Artificial Intelligence, A Modern Approach</i> by Russell and Norvig. In brief, the algorithm considers two
 *  sentences \f$p\f$ and \f$q\f$ and returns a variable substitution map, or <i>unifier</i> if one exists:
 *  \f[
 *      \text{Unify}(p, q) \mathrel{\vcenter{:}}= \theta \text{ such that }
 *      \text{Sub}(\theta, p) = \text{Sub}(\theta, q).
 *  \f]
 *  For brevity, the definition of our sameness metric is not elaborated here.
 */
class UnifyCandidateBase
{
public:
    virtual ~UnifyCandidateBase() = default;

    /**
     * @brief Attempts to unify the candidate with a PredicationNode
     * @param predicate The PredicationNode unification target
     * @return A unifying variable-substitution map, if one exists. If no unification can be made, empty is returned.
     */
    [[nodiscard]] std::optional<Substitution> unify(const PredicationNode &predicate);

    /**
     * @brief Attempts to unify the candidate with a generic ITermNode
     * @param variable The ITermNode unification target
     * @return A unifying variable-substitution map, if one exists. If no unification can be made, empty is returned.
     */
    [[nodiscard]] std::optional<Substitution> unify(const ITermNode &variable);

    [[nodiscard]] virtual bool unify_work(const ITermNode &term);

    [[nodiscard]] virtual bool unify_work(const PredicationNode &predicate);

    [[nodiscard]] virtual bool unify_work(const FunctionNode &function);

    [[nodiscard]] virtual bool unify_work(const VariableNode &variable);

protected:
    std::optional<Substitution> unify_substitution;
};

}

#endif
