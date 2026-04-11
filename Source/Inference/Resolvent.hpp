/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL Resolution resolvent
 * @author Oliver Dixon
 * @date 2026-01-25
 * @version Development
 */

#ifndef OPTIFOL_RESOLVENT_HPP
#define OPTIFOL_RESOLVENT_HPP

#include "ProofTreeNode.hpp"
#include "ResolventQueue.hpp"
#include "Unifier.hpp"

namespace optifol
{

/**
 * @class Resolvent
 * @brief A Resolvent is a ProofTreeNode containing a LHS and RHS parent, a resolvent Clause deduced from the parents,
 *  and a Unifier map to induce the resolution.
 */
class Resolvent : public IHashable,
                  public ProofTreeNode
{
public:
    Resolvent(const ProofTreeNode *lhs_parent, const ProofTreeNode *rhs_parent, Unifier unifier,
            const Clause *resolution);

    Resolvent(Resolvent &&) = default;
    Resolvent &operator=(Resolvent &&) = default;

    [[nodiscard]] bool operator<(const Resolvent &other) const noexcept;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] bool operator==(const Resolvent &other) const;

    [[nodiscard]] bool is_unit() const noexcept override;

    [[nodiscard]] const Clause *observe_node() const noexcept override;

    [[nodiscard]] std::optional<const Unifier *> observe_edge() const noexcept override;

private:
    friend void ResolventQueue::push(Resolvent, std::unique_ptr<Clause> &&);

    Unifier unifier;
    const Clause *resolution;
};

} // namespace optifol

#endif // OPTIFOL_RESOLVENT_HPP
