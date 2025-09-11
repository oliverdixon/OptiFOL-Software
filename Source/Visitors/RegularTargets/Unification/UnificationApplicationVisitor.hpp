/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 9/11/25.
//

#ifndef OPTIFOL_UNIFICATIONAPPLICATIONVISITOR_HPP
#define OPTIFOL_UNIFICATIONAPPLICATIONVISITOR_HPP

#include "UnificationVisitor.hpp"

namespace optifol
{

class BinaryConnected;
class Identity;
class Literal;
class SentenceRoot;

class UnificationApplicationVisitor
{
public:
    using VisitorReturn = std::variant<std::unique_ptr<IProcessedTerm>, const IProcessedTerm *>;

    UnificationApplicationVisitor(const UnificationVisitor::SubstitutionMap& substitutions,
        std::shared_ptr<SymbolRepository> symbol_repository);

    static VisitorReturn visit(const Constant &node);

    VisitorReturn visit(const Variable &node) const;

    VisitorReturn visit(const Function &node) const;

private:
    const UnificationVisitor::SubstitutionMap& substitutions;
    const std::shared_ptr<SymbolRepository> symbol_repository;
};

} // namespace optifol

#endif // OPTIFOL_UNIFICATIONAPPLICATIONVISITOR_HPP
