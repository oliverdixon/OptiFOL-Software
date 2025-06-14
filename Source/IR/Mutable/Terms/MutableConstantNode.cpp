/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Constant Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableConstantNode.hpp"

#include "../../../Visitors/Terms/MutatingTermVisitorBase.hpp"

namespace optifol
{

MutableConstantNode::MutableConstantNode(std::string name) :
    name(std::move(name))
{}

std::unique_ptr<IMutableTermNode> MutableConstantNode::clone() const
{
    return std::make_unique<MutableConstantNode>(name);
}

std::string MutableConstantNode::to_string() const
{
    return name;
}

std::string MutableConstantNode::get_disambiguated_name() const
{
    return to_string();
}

void MutableConstantNode::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

std::ostream &MutableConstantNode::serialise(std::ostream &ostream) const
{
    return ostream << name;
}

} // namespace optifol
