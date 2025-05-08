/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Skolem Function Term IR node
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#include "SkolemFunctionNode.hpp"

namespace optifol
{

SkolemFunctionNode::SkolemFunctionNode(std::string name,
        std::vector<std::unique_ptr<ITermNode>> &&quantified_variables) :
    FunctionNode(std::move(name), std::move(quantified_variables))
{}

SkolemFunctionNode::SkolemFunctionNode(std::string name,
        const std::vector<std::unique_ptr<ITermNode>> &quantified_variables) :
    FunctionNode(std::move(name), quantified_variables)
{}

std::unique_ptr<ITermNode> SkolemFunctionNode::clone() const
{
    std::vector<std::unique_ptr<ITermNode>> cloned_arguments;

    cloned_arguments.reserve(arguments.size());
    for (const auto& argument : arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<SkolemFunctionNode>(name, std::move(cloned_arguments));
}

std::string SkolemFunctionNode::get_disambiguated_name() const
{
    return '{' + name + '}';
}

}
