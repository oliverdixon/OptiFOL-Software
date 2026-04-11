/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/29/26.
//

#ifndef OPTIFOL_PROOFTREENODE_HPP
#define OPTIFOL_PROOFTREENODE_HPP

#include <memory>

#include "../ISerialisable.hpp"

namespace optifol
{

struct Unifier;
class Clause;
class Resolvent;

class ProofTreeNode : public ISerialisable
{
public:
    ProofTreeNode(const ProofTreeNode &) = default;
    ProofTreeNode &operator=(const ProofTreeNode &) = default;

    [[nodiscard]] const ProofTreeNode *observe_lhs_parent() const noexcept
    {
        return lhs_parent;
    }

    [[nodiscard]] const ProofTreeNode *observe_rhs_parent() const noexcept
    {
        return rhs_parent;
    }

    [[nodiscard]] virtual const Clause *observe_node() const noexcept = 0;

    [[nodiscard]] virtual std::optional<const Unifier *> observe_edge() const noexcept = 0;

    [[nodiscard]] unsigned int get_depth() const noexcept
    {
        return depth;
    }

    [[nodiscard]] virtual bool is_unit() const noexcept = 0;

protected:
    ProofTreeNode() = default;

    ProofTreeNode(const ProofTreeNode *const lhs_parent, const ProofTreeNode *const rhs_parent) :
        lhs_parent(lhs_parent),
        rhs_parent(rhs_parent),
        depth(std::max(lhs_parent->get_depth(), rhs_parent->get_depth()) + 1)
    {
    }

private:
    const ProofTreeNode *lhs_parent = nullptr;
    const ProofTreeNode *rhs_parent = nullptr;
    unsigned int depth = 0;
};

} // namespace optifol

#endif // OPTIFOL_PROOFTREENODE_HPP
