/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL Resolution Knowledge Base
 * @author Oliver Dixon
 * @date 2025-09-14
 * @version Development
 */

#ifndef OPTIFOL_KNOWLEDGEBASE_HPP
#define OPTIFOL_KNOWLEDGEBASE_HPP

#include <log4cxx/logger.h>

#include "../IR/Sentences/SentenceRoot.hpp"
#include "Resolvent.hpp"

namespace optifol
{

class SymbolRepository;
class Variable;
class IProcessedTerm;

class KnowledgeBase
{
public:
    explicit KnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository);

    void tell(const SentenceRoot& sentence);

    void tell(const Clause& clause);

    bool query(const SentenceRoot &negated_query);

private:
    static void collect_unified_literals(const Literal &self, const Clause &source_clause, Clause &destination_clause,
            UnificationApplicationVisitor &applicator);

    static Clause factor_literals(
            const Clause &unified_clause, UnificationVisitor &unifier, UnificationApplicationVisitor &applicator);

    [[nodiscard]] std::vector<Resolvent> find_resolvents(const Clause &lhs_clause, const Clause &rhs_clause) const;

    static const log4cxx::LoggerPtr kb_logger;
    static const log4cxx::LoggerPtr resolution_logger;
    static const log4cxx::LoggerPtr factoring_logger;

    std::vector<Clause> clauses;
    std::shared_ptr<SymbolRepository> symbol_repository;
};

} // namespace optifol

#endif // OPTIFOL_KNOWLEDGEBASE_HPP
