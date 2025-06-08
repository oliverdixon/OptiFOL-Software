/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class definition for the generic IR Term interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "ITermNode.hpp"
#include "../../IR/Terms/FunctionNode.hpp"
#include "../../Visitors/Unification/UnificationVisitor.hpp"

namespace optifol
{

std::size_t ITermNode::hash() const noexcept
{
    return std::hash<std::string>{}(get_disambiguated_name());
}

bool ITermNode::accept(UnificationVisitor &visitor, const VariableNode &target) const
{
    // Justification: unification has the commutative property.
    return visitor.visit(target, *this);
}

bool ITermNode::accept(UnificationVisitor &visitor, const ITermNode &target) const
{
    std::ignore = visitor;
    return optifol::UnificationVisitor::visit(*this, target);
}

bool ITermNode::accept(UnificationVisitor &visitor, const FunctionNode &target) const
{
    std::ignore = visitor;
    return optifol::UnificationVisitor::visit(*this, target);
}

}
