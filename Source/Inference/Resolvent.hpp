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

#include "../IR/Sentences/SentenceRoot.hpp"
#include "ProofTreeNode.hpp"
#include "ResolventQueue.hpp"
#include "Unifier.hpp"

namespace optifol
{

class Resolvent :
    public IHashable,
    public ProofTreeNode
{
public:
    Resolvent(const ProofTreeNode * lhs_parent, const ProofTreeNode * rhs_parent, Unifier unifier,
        const Clause * resolution);

    Resolvent(Resolvent&&) = default;
    Resolvent& operator=(Resolvent&&) = default;

    [[nodiscard]] bool operator<(const Resolvent& other) const noexcept;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] bool operator==(const Resolvent &other) const;

    [[nodiscard]] bool is_unit() const noexcept override;

    [[nodiscard]] const Clause * observe_substance() const noexcept override;

private:
    friend void ResolventQueue::push(Resolvent, std::unique_ptr<Clause> &&);

    Unifier unifier;
    const Clause * resolution;
};

} // namespace optifol

#endif // OPTIFOL_RESOLVENT_HPP
