/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Constant Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "ConstantNode.hpp"

#include "../../Visitors/Terms/MutatingTermVisitorBase.hpp"

namespace optifol
{

ConstantNode::ConstantNode(std::string name) :
    name(std::move(name))
{}

std::unique_ptr<ITermNode> ConstantNode::clone() const
{
    return std::make_unique<ConstantNode>(name);
}

std::string ConstantNode::to_string() const
{
    return name;
}

std::string ConstantNode::get_disambiguated_name() const
{
    return to_string();
}

void ConstantNode::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

std::ostream &ConstantNode::serialise(std::ostream &ostream) const
{
    return ostream << name;
}

} // namespace optifol
