/*
 * Copyright (c) All Rights Reserved
 * 2024-2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file MutatingTermVisitorBase.cpp
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
    auto& arguments = node.get_arguments();

    for (const auto& argument : arguments)
        argument->accept(*this);
}

void MutatingTermVisitorBase::visit(ConstantNode &node)
{
    (void) node;
}

void MutatingTermVisitorBase::visit(VariableNode &node)
{
    (void) node;
}

}
