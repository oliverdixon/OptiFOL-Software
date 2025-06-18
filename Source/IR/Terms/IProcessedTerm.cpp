/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Default class implementation for the IR Processed Term interface
 * @author Oliver Dixon
 * @date 2025-06-14
 * @version Development
 */

#include "IProcessedTerm.hpp"
#include "../../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

bool IProcessedTerm::accept(UnificationVisitor &visitor, const IProcessedTerm &target) const
{
    std::ignore = visitor;
    return UnificationVisitor::visit(*this, target);
}

bool IProcessedTerm::accept(UnificationVisitor &visitor, const Function &target) const
{
    std::ignore = visitor;
    return UnificationVisitor::visit(*this, target);
}

} // namespace optifol
