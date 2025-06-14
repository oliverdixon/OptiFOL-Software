/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Mutating Term Visitor base class.
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#include "MutatingTermVisitorBase.hpp"
#include "../../IR/Mutable/Terms/MutableFunctionNode.hpp"

namespace optifol
{

void MutatingTermVisitorBase::visit(MutableFunctionNode &node)
{
    const auto& arguments = node.observe_arguments();

    for (const auto& argument : arguments)
        argument->accept(*this);
}

void MutatingTermVisitorBase::visit(MutableConstantNode &node)
{
    std::ignore = node;
}

void MutatingTermVisitorBase::visit(MutableVariableNode &node)
{
    std::ignore = node;
}

void MutatingTermVisitorBase::visit(MutableSkolemFunctionNode &node)
{
    std::ignore = node;
}

}
