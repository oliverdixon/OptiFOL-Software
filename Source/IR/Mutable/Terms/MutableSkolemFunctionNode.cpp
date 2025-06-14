/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Skolem Function Term IR node
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#include "MutableSkolemFunctionNode.hpp"

namespace optifol
{

MutableSkolemFunctionNode::MutableSkolemFunctionNode(std::string name,
        std::vector<std::unique_ptr<IMutableTermNode>> &&quantified_variables) :
    MutableFunctionNode(std::move(name), std::move(quantified_variables))
{}

MutableSkolemFunctionNode::MutableSkolemFunctionNode(std::string name,
        const std::vector<std::unique_ptr<IMutableTermNode>> &quantified_variables) :
    MutableFunctionNode(std::move(name), quantified_variables)
{}

std::unique_ptr<IMutableTermNode> MutableSkolemFunctionNode::clone() const
{
    std::vector<std::unique_ptr<IMutableTermNode>> cloned_arguments;

    cloned_arguments.reserve(arguments.size());
    for (const auto& argument : arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<MutableSkolemFunctionNode>(name, std::move(cloned_arguments));
}

std::string MutableSkolemFunctionNode::get_disambiguated_name() const
{
    return '{' + name + '}';
}

}
