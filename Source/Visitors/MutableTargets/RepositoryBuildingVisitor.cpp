/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Symbol Repository-Building Visitor
 * @author Oliver Dixon
 * @date 2025-06-22
 * @version Development
 */

#include "RepositoryBuildingVisitor.hpp"

namespace optifol
{

const char *RepositoryBuildingVisitor::visitor_name = "Repository-Building Visitor";

RepositoryBuildingVisitor::RepositoryBuildingVisitor(SymbolRepository &symbol_repository) :
    symbol_repository(symbol_repository)
{
}

std::string_view RepositoryBuildingVisitor::get_visitor_name()
{
    return visitor_name;
}

void RepositoryBuildingVisitor::visit(MutableQuantified &node)
{
}

void RepositoryBuildingVisitor::visit(MutableBinaryConnected &node)
{
}

void RepositoryBuildingVisitor::visit(MutableIdentity &node)
{
}

void RepositoryBuildingVisitor::visit(MutablePredicate &predicate)
{
}

void RepositoryBuildingVisitor::visit(MutableSentenceRoot &node)
{
}

void RepositoryBuildingVisitor::visit(IMutableTerm &node)
{
}

} // namespace optifol
