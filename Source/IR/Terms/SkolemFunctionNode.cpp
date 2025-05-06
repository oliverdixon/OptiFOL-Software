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

std::string SkolemFunctionNode::get_disambiguated_name() const
{
    return '{' + FunctionNode::get_disambiguated_name() + '}';
}

}
