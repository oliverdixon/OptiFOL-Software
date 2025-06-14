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
#include "../../../IR/MutableVariants/Terms/MutableFunction.hpp"

namespace optifol
{

void MutatingTermVisitorBase::visit(MutableFunction &node)
{
    const auto& arguments = node.observe_arguments();

    for (const auto& argument : arguments)
        argument->accept(*this);
}

void MutatingTermVisitorBase::visit(MutableConstant &node)
{
    std::ignore = node;
}

void MutatingTermVisitorBase::visit(MutableVariable &node)
{
    std::ignore = node;
}

void MutatingTermVisitorBase::visit(MutableSkolemFunction &node)
{
    std::ignore = node;
}

}
