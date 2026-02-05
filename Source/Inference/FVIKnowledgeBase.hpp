/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 2/4/26.
//

#ifndef OPTIFOL_FVIKNOWLEDGEBASE_HPP
#define OPTIFOL_FVIKNOWLEDGEBASE_HPP

#include <map>
#include <memory>
#include <vector>

#include "Feature.hpp"

namespace optifol
{

class SymbolRepository;
class Clause;

class FVIKnowledgeBase
{
public:
    explicit FVIKnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository);

    void add_clause(const Clause *clause);

    // Forward subsumption
    std::vector<const Clause *> get_subsuming(const Clause &clause) const;

    // Backward subsumption
    std::vector<const Clause *> get_subsumed(const Clause &clause) const;

    void remove_subsumed(const Clause& clause);

    void replace_subsumed(const Clause *clause);

private:
    struct FVINode
    {
        std::map<Feature, std::unique_ptr<FVINode>> children;
        std::vector<const Clause *> clauses;
    };

    void get_subsuming(const Clause &clause, const FVINode &node, unsigned int depth,
            std::vector<const Clause *> &subsuming_clauses) const;

    void get_subsumed(const Clause &clause, const FVINode &node, unsigned int depth,
            std::vector<const Clause *> &subsumed_clauses) const;

    void explore_leaf(const Clause& clause, const FVINode& node, std::vector<const Clause *>& subsumed_clauses) const;

    void remove_subsumed(const Clause &clause, FVINode &node, unsigned int depth);

    void explore_and_remove_leaf(const Clause &clause, FVINode &node);

    FVINode root;
    std::shared_ptr<SymbolRepository> symbol_repository;
};

} // namespace optifol

#endif // OPTIFOL_FVIKNOWLEDGEBASE_HPP
