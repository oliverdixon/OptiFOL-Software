/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic mutable IR Term interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "IMutableTermNode.hpp"
#include "../../Mutable/Terms/MutableFunctionNode.hpp"
#include "../../../Visitors/Unification/UnificationVisitor.hpp"

namespace optifol
{

std::size_t IMutableTermNode::hash() const noexcept
{
    return std::hash<std::string>{}(get_disambiguated_name());
}

bool IMutableTermNode::accept(UnificationVisitor &visitor, const MutableVariableNode &target) const
{
    // Justification: unification has the commutative property.
    return visitor.visit(target, *this);
}

bool IMutableTermNode::accept(UnificationVisitor &visitor, const IMutableTermNode &target) const
{
    std::ignore = visitor;
    return optifol::UnificationVisitor::visit(*this, target);
}

bool IMutableTermNode::accept(UnificationVisitor &visitor, const MutableFunctionNode &target) const
{
    std::ignore = visitor;
    return optifol::UnificationVisitor::visit(*this, target);
}

}
