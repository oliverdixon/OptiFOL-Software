/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
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

}
