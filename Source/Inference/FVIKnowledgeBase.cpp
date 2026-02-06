/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 2/4/26.
//

#include "FVIKnowledgeBase.hpp"

#include <ranges>

#include "../IR/Sentences/Clause.hpp"
#include "../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

FVIKnowledgeBase::FVIKnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository))
{
}

std::pair<UniqueUnorderedSet<Clause>::iterator, bool> FVIKnowledgeBase::add_clause(std::unique_ptr<Clause> &&clause)
{
    FVINode * node = &root;
    const auto& clause_features = clause->observe_features();

    for (const auto& feature : clause_features) {
        auto& child = node->children[feature];
        if (child == nullptr)
            child = std::make_unique<FVINode>();
        node = child.get();
    }

    auto insertion_result = node->clause_set.insert(std::move(clause));
    if (insertion_result.second)
        ++total_clause_count;

    return insertion_result;
}

std::vector<const Clause *> FVIKnowledgeBase::get_subsuming(const Clause &clause) const
{
    std::vector<const Clause *> subsuming_clauses;
    get_subsuming(clause, root, 0, subsuming_clauses);
    return subsuming_clauses;
}

std::vector<const Clause *> FVIKnowledgeBase::get_subsumed(const Clause &clause) const
{
    std::vector<const Clause *> subsumed_clauses;
    get_subsumed(clause, root, 0, subsumed_clauses);
    return subsumed_clauses;
}

void FVIKnowledgeBase::remove_subsumed(const Clause &clause)
{
    remove_subsumed(clause, root, 0);
}

void FVIKnowledgeBase::replace_subsumed(std::unique_ptr<Clause> &&clause)
{
    const auto subsuming = get_subsuming(*clause);
    if (!subsuming.empty())
        return;

    remove_subsumed(*clause);
    add_clause(std::move(clause));
}

std::generator<const Clause *> FVIKnowledgeBase::flatten() const
{
    return flatten(root);
}

std::generator<const Clause *> FVIKnowledgeBase::flatten(const FVINode &node)
{
    for (const auto& my_clause : node.clause_set)
        co_yield my_clause.get();

    for (const auto& [child_feature, child_node] : node.children) {
        std::ignore = child_feature;
        for (const auto child_clause : flatten(*child_node))
            co_yield child_clause;
    }
}

void FVIKnowledgeBase::get_subsuming(const Clause &clause, const FVINode &node, const unsigned int depth,
        std::vector<const Clause *> &subsuming_clauses) const
{
    const auto& features = clause.observe_features();
    if (depth >= features.size())

        // The given node is a leaf node.
        for (const auto contained_clause : node.clause_set | unwrap_clause) {
            auto visitor = UnificationVisitor(symbol_repository);
            if (contained_clause->subsumes(clause, visitor))
                subsuming_clauses.push_back(contained_clause);
            else
                return;
        }

    else {

        // The given node is not a leaf node.
        const auto& feature = features[depth];
        const auto filter_unrelated_features = std::ranges::views::drop_while(
            [&feature](const auto& candidate_pair)
            {
                return candidate_pair.first < feature;
            });

        const auto select_lesser_magnitudes = std::ranges::views::take_while(
            [&feature](const auto& candidate_pair)
            {
                return candidate_pair.first.get_feature_type() == feature.get_feature_type() &&
                    candidate_pair.first.get_magnitude() <= feature.get_magnitude();
            });

        for (const auto& [child_feature, child_node] : node.children |
                filter_unrelated_features | select_lesser_magnitudes)
            get_subsuming(clause, *child_node, depth + 1, subsuming_clauses);

        get_subsuming(clause, node, depth + 1, subsuming_clauses);
    }
}

void FVIKnowledgeBase::get_subsumed(const Clause &clause, const FVINode &node, const unsigned int depth,
        std::vector<const Clause *> &subsumed_clauses) const
{
    const auto& features = clause.observe_features();

    if (depth >= features.size())

        // The given node is a leaf node.
        explore_leaf(clause, node, subsumed_clauses);

    else {

        // The given node is not a leaf node.
        const auto filter_greater_magnitudes = std::ranges::views::take_while(
            [depth, &features](const auto& candidate_pair)
            {
                if (depth == 0)
                    return true;

                return candidate_pair.first > features[depth - 1];
            }
        );

        const auto& current_feature = features[depth];
        for (const auto& [child_feature, child_node] : node.children |
                std::ranges::views::reverse | filter_greater_magnitudes)
            if (child_feature.get_feature_type() <= current_feature.get_feature_type()) {
                const std::size_t offset = child_feature.get_feature_type() == current_feature.get_feature_type() &&
                    child_feature.get_magnitude() >= current_feature.get_magnitude() ? 1 : 0;

                get_subsumed(clause, *child_node, depth + offset, subsumed_clauses);
            }

    }
}

void FVIKnowledgeBase::explore_leaf(
        const Clause &clause, const FVINode &node, std::vector<const Clause *> &subsumed_clauses) const
{
    for (const auto contained_clause : node.clause_set | unwrap_clause) {
        auto visitor = UnificationVisitor(symbol_repository);
        if (clause.subsumes(*contained_clause, visitor))
            subsumed_clauses.push_back(contained_clause);
    }

    for (const auto& [feature, child_node] : node.children)
        explore_leaf(clause, *child_node, subsumed_clauses);
}

void FVIKnowledgeBase::remove_subsumed(const Clause &clause, FVINode &node, unsigned int depth)
{
    const auto& features = clause.observe_features();

    if (depth >= features.size())

        // The given node is a leaf node.
        explore_and_remove_leaf(clause, node);

    else {

        // The given node is not a leaf node.
        const auto filter_greater_magnitudes = std::ranges::views::take_while(
            [depth, &features](const auto& candidate_pair)
            {
                if (depth == 0)
                    return true;

                return candidate_pair.first > features[depth - 1];
            }
        );

        const auto& current_feature = features[depth];
        std::vector<Feature> slated_for_removal;

        for (const auto& [child_feature, child_node] : node.children |
                std::ranges::views::reverse | filter_greater_magnitudes)
            if (child_feature.get_feature_type() <= current_feature.get_feature_type()) {
                const std::size_t offset = child_feature.get_feature_type() == current_feature.get_feature_type() &&
                    child_feature.get_magnitude() >= current_feature.get_magnitude() ? 1 : 0;

                remove_subsumed(clause, *child_node, depth + offset);
                if (child_node->children.empty() && child_node->clause_set.empty())
                    slated_for_removal.push_back(child_feature);
            }

        for (const auto& target : slated_for_removal)
            node.children.erase(target);
    }
}

void FVIKnowledgeBase::explore_and_remove_leaf(const Clause &clause, FVINode &node)
{
    unsigned int erased_count = 0;

    std::erase_if(node.clause_set, [this, &clause, &erased_count](const auto& contained_clause)
    {
        auto visitor = UnificationVisitor(symbol_repository);
        if (clause.subsumes(*contained_clause, visitor)) {
            ++erased_count;
            return true;
        }

        return false;
    });

    total_clause_count -= erased_count;

    std::vector<Feature> slated_for_removal;
    for (const auto& [child_feature, child_node] : node.children) {
        explore_and_remove_leaf(clause, *child_node);
        if (child_node->children.empty() && child_node->clause_set.empty())
            slated_for_removal.push_back(child_feature);
    }

    for (const auto& target : slated_for_removal)
        node.children.erase(target);
}

unsigned int FVIKnowledgeBase::get_total_clause_count() const noexcept
{
    return total_clause_count;
}

} // namespace optifol
