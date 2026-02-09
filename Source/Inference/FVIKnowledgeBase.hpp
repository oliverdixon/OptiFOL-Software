/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 2/4/26.
//

#ifndef OPTIFOL_FVIKNOWLEDGEBASE_HPP
#define OPTIFOL_FVIKNOWLEDGEBASE_HPP

#include <generator>
#include <map>
#include <memory>
#include <vector>

#include "../Optifol.hpp"
#include "../IR/Sentences/Clause.hpp"
#include "Feature.hpp"

namespace optifol
{

class SymbolRepository;

class FVIKnowledgeBase
{
public:
    explicit FVIKnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository);

    FVIKnowledgeBase(const FVIKnowledgeBase& other_kb);

    std::pair<UniqueUnorderedSet<Clause>::iterator, bool> add_clause(std::unique_ptr<Clause> &&clause);

    // Forward subsumption
    std::vector<const Clause *> get_subsuming(const Clause &clause) const;

    // Backward subsumption
    std::vector<const Clause *> get_subsumed(const Clause &clause) const;

    void remove_subsumed(const Clause& clause);

    std::pair<UniqueUnorderedSet<Clause>::iterator, bool> replace_subsumed(std::unique_ptr<Clause> &&clause);

    std::generator<const Clause *> flatten() const;

    [[nodiscard]] unsigned int get_total_clause_count() const noexcept;

private:
    struct FVINode
    {
        std::map<Feature, std::unique_ptr<FVINode>> children;
        UniqueUnorderedSet<Clause> clause_set;

        FVINode() = default;

        FVINode(const FVINode& src_node);
        FVINode& operator=(const FVINode& src_node);
    };

    static constexpr auto unwrap_clause = std::views::transform(
        [](const std::unique_ptr<Clause>& clause){ return clause.get(); });

    static std::generator<const Clause *> flatten(const FVINode& node);

    void get_subsuming(const Clause &clause, const FVINode &node, unsigned int depth,
            std::vector<const Clause *> &subsuming_clauses) const;

    void get_subsumed(const Clause &clause, const FVINode &node, unsigned int depth,
            std::vector<const Clause *> &subsumed_clauses) const;

    void explore_leaf(const Clause& clause, const FVINode& node, std::vector<const Clause *>& subsumed_clauses) const;

    void remove_subsumed(const Clause &clause, FVINode &node, unsigned int depth);

    void explore_and_remove_leaf(const Clause &clause, FVINode &node);

    FVINode root;
    std::shared_ptr<SymbolRepository> symbol_repository;
    unsigned int total_clause_count = 0;
    std::vector<std::unique_ptr<Clause>> orphaned_clauses;
};

} // namespace optifol

#endif // OPTIFOL_FVIKNOWLEDGEBASE_HPP
