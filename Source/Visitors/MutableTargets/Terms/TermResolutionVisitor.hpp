/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Term-Resolution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef TERMRESOLUTIONVISITOR_HPP
#define TERMRESOLUTIONVISITOR_HPP

#include <string>
#include <unordered_map>

#include "../../../IR/MutableVariants/Terms/IMutableTerm.hpp"
#include "MutatingTermVisitorBase.hpp"

namespace optifol
{

class MutableVariable;

/**
 * @class TermResolutionVisitor
 * @brief The TermResolutionVisitor rewrites a term-based model according to a variable set of substitution
 * rules.
 */
class TermResolutionVisitor : public MutatingTermVisitorBase
{
public:
    /**
     * @brief Construct a new TermResolutionVisitor with a weak observing reference to the substitution rules
     * @param rewriting_rules_hook A weak reference, guaranteed to persist for the lifetime of the
     * TermResolutionVisitor object, mapping term names to replacement terms. Keys are views and have the same
     * lifetime guarantees as the map.
     */
    explicit TermResolutionVisitor(
            const std::unordered_map<std::string, std::unique_ptr<IMutableTerm>, StringHash, std::equal_to<>>
                    &rewriting_rules_hook);

    /**
     * @brief Rewrite a function and all arguments according to the substitution ruleset.
     * @param node The function node to rewrite
     * @details Given an @f$ N @f$-ary function @f$ f \left( x_1, \ldots, x_N \right) @f$, the visitor
     * rewrites the model to produce @f$ f^\prime \left( x_1^\prime, \ldots, x_N^\prime \right) @f$ over the
     * ruleset @f$ V @f$ where
     *  @f[
     *      \alpha =
     *      \begin{cases}
     *          \alpha^\prime & \text{if } \left\{ \alpha \mapsto \alpha^\prime \right\} \in V \text{; or} \\
     *          \alpha & \text{otherwise}
     *      \end{cases}
     *  @f]
     *  for @f$ \alpha \in \left\{ f, x_1, \ldots, x_N \right\} @f$.
     */
    void visit(MutableFunction &node) override;

private:
    const std::unordered_map<std::string, std::unique_ptr<IMutableTerm>, StringHash, std::equal_to<>>
            &rewriting_rules_hook;
};

} // namespace optifol

#endif
