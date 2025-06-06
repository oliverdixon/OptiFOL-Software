/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Variable Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "VariableNode.hpp"

#include "../../Visitors/Terms/MutatingTermVisitorBase.hpp"

namespace optifol
{

VariableNode::VariableNode(std::string name):
    name(std::move(name))
{}

VariableNode::VariableNode(std::string name, const std::string &disambiguated_name) :
    name(std::move(name)),
    disambiguated_name(disambiguated_name)
{}

std::unique_ptr<ITermNode> VariableNode::clone() const
{
    if (disambiguated_name.has_value())
        return std::make_unique<VariableNode>(name, *disambiguated_name);

    return std::make_unique<VariableNode>(name);
}

std::string VariableNode::to_string() const
{
    return name;
}

std::string VariableNode::get_disambiguated_name() const
{
    return disambiguated_name.value_or(name);
}

void VariableNode::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

} // namespace optifol
