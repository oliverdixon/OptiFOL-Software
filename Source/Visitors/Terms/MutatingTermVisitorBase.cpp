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
#include "../../IR/Terms/FunctionNode.hpp"

namespace optifol
{

void MutatingTermVisitorBase::visit(FunctionNode &node)
{
    const auto& arguments = node.observe_arguments();

    for (const auto& argument : arguments)
        argument->accept(*this);
}

void MutatingTermVisitorBase::visit(ConstantNode &node)
{
    std::ignore = node;
}

void MutatingTermVisitorBase::visit(VariableNode &node)
{
    std::ignore = node;
}

void MutatingTermVisitorBase::visit(SkolemFunctionNode &node)
{
    std::ignore = node;
}

}
