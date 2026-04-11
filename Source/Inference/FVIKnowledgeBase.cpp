/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Feature Vector Indexing Clause knowledge base
 * @author Oliver Dixon
 * @date 2026-02-09
 * @version Development
 */

#include "FVIKnowledgeBase.hpp"

#include <ranges>

#include "../IR/Sentences/Clause.hpp"
#include "../Logging.hpp"
#include "../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

const log4cxx::LoggerPtr FVIKnowledgeBase::kb_logger =
        Logging::get_logger({"LogicServices", "FVIKnowledgeBase"});

FVIKnowledgeBase::FVIKnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository)),
    unification_visitor(this->symbol_repository)
{
}

std::pair<UniqueUnorderedSet<Clause>::iterator, bool> FVIKnowledgeBase::insert_clause(
        std::unique_ptr<Clause> &&clause,
        const std::optional<std::function<void(std::unique_ptr<Clause> &&)>> &rejection_handler)
{
    FVINode *node = &root;
    const auto &clause_features = clause->observe_features();

    for (const auto &feature: clause_features) {
        auto &child = node->children[feature];
        if (child == nullptr)
            child = std::make_unique<FVINode>();
        node = child.get();
    }

    if (node->clause_set.contains(clause)) {
        LOG4CXX_INFO(kb_logger,
                std::format("Rejecting clause {} because it already exists in the KB trie.", *clause));
        if (rejection_handler.has_value())
            (*rejection_handler)(std::move(clause));
        return {node->clause_set.end(), false};
    }

    LOG4CXX_INFO(kb_logger, std::format("Accepting clause {} into the KB.", *clause));
    auto [it, success] = node->clause_set.insert(std::move(clause));
    assert(success);
    ++total_clause_count;
    LOG4CXX_DEBUG(kb_logger, std::format("The KB now contains {} clauses.", total_clause_count));
    return {it, success};
}

std::pair<UniqueUnorderedSet<Clause>::iterator, bool> FVIKnowledgeBase::add_clause(
        std::unique_ptr<Clause> &&clause,
        const std::optional<std::function<void(std::unique_ptr<Clause> &&)>> &rejection_handler)
{
    std::vector<const Clause *> subsuming_clauses;
    get_subsuming(*clause, root, 0, subsuming_clauses); // TODO don't need to build up a whole vector here...

    if (!subsuming_clauses.empty()) {
        LOG4CXX_INFO(kb_logger,
                std::format("Rejecting clause {} because it would be subsumed by current KB.", *clause));
        if (rejection_handler.has_value())
            (*rejection_handler)(std::move(clause));
        return {root.clause_set.end(), false};
    }

    remove_subsumed(*clause, root, 0);
    return insert_clause(std::move(clause), rejection_handler);
}

std::generator<const Clause *> FVIKnowledgeBase::flatten() const
{
    std::vector<const FVINode *> stack;
    stack.push_back(&root);

    while (!stack.empty()) {
        const auto node = stack.back();
        stack.pop_back();

        for (const auto &clause: node->clause_set)
            co_yield clause.get();

        for (const auto &[_, child_node]: node->children)
            stack.push_back(child_node.get());
    }
}

void FVIKnowledgeBase::get_subsuming(const Clause &clause, const FVINode &node, const unsigned int depth,
        std::vector<const Clause *> &subsuming_clauses) const
{
    const auto &features = clause.observe_features();
    if (depth >= features.size())

        // The given node is a leaf node.
        for (const auto contained_clause: node.clause_set | unwrap_clause) {
            const auto subsumes = contained_clause->subsumes(clause, unification_visitor);
            unification_visitor.reset_substitutions();
            if (subsumes)
                subsuming_clauses.push_back(contained_clause);
            else
                return;
        }

    else {

        // The given node is not a leaf node.
        const auto &feature = features[depth];
        const auto filter_unrelated_features = std::ranges::views::drop_while(
                [&feature](const auto &candidate_pair) { return candidate_pair.first < feature; });

        const auto select_lesser_magnitudes = std::ranges::views::take_while(
                [&feature](const auto &candidate_pair)
                {
                    return candidate_pair.first.get_feature_type() == feature.get_feature_type() &&
                            candidate_pair.first.get_magnitude() <= feature.get_magnitude();
                });

        for (const auto &[child_feature, child_node]:
                node.children | filter_unrelated_features | select_lesser_magnitudes)
            get_subsuming(clause, *child_node, depth + 1, subsuming_clauses);

        get_subsuming(clause, node, depth + 1, subsuming_clauses);
    }
}

void FVIKnowledgeBase::get_subsumed(const Clause &clause, const FVINode &node, const unsigned int depth,
        std::vector<const Clause *> &subsumed_clauses) const
{
    const auto &features = clause.observe_features();

    if (depth >= features.size())

        // The given node is a leaf node.
        explore_leaf(clause, node, subsumed_clauses);

    else {

        // The given node is not a leaf node.
        const auto filter_greater_magnitudes = std::ranges::views::take_while(
                [depth, &features](const auto &candidate_pair)
                {
                    if (depth == 0)
                        return true;

                    return candidate_pair.first > features[depth - 1];
                });

        const auto &current_feature = features[depth];
        for (const auto &[child_feature, child_node]:
                node.children | std::ranges::views::reverse | filter_greater_magnitudes)
            if (child_feature.get_feature_type() <= current_feature.get_feature_type()) {
                const std::size_t offset =
                        child_feature.get_feature_type() == current_feature.get_feature_type() &&
                                child_feature.get_magnitude() >= current_feature.get_magnitude()
                        ? 1
                        : 0;

                get_subsumed(clause, *child_node, depth + offset, subsumed_clauses);
            }
    }
}

void FVIKnowledgeBase::explore_leaf(
        const Clause &clause, const FVINode &node, std::vector<const Clause *> &subsumed_clauses) const
{
    for (const auto contained_clause: node.clause_set | unwrap_clause) {
        const auto subsumes = clause.subsumes(*contained_clause, unification_visitor);
        unification_visitor.reset_substitutions();
        if (subsumes)
            subsumed_clauses.push_back(contained_clause);
    }

    for (const auto &[feature, child_node]: node.children)
        explore_leaf(clause, *child_node, subsumed_clauses);
}

void FVIKnowledgeBase::remove_subsumed(const Clause &clause, FVINode &node, unsigned int depth)
{
    const auto &features = clause.observe_features();

    if (depth >= features.size())

        // The given node is a leaf node.
        explore_and_remove_leaf(clause, node);

    else {

        // The given node is not a leaf node.
        const auto filter_greater_magnitudes = std::ranges::views::take_while(
                [depth, &features](const auto &candidate_pair)
                {
                    if (depth == 0)
                        return true;

                    return candidate_pair.first > features[depth - 1];
                });

        const auto &current_feature = features[depth];
        std::vector<Feature> slated_for_removal;

        for (const auto &[child_feature, child_node]:
                node.children | std::ranges::views::reverse | filter_greater_magnitudes)

            if (child_feature.get_feature_type() <= current_feature.get_feature_type()) {
                const std::size_t offset =
                        child_feature.get_feature_type() == current_feature.get_feature_type() &&
                                child_feature.get_magnitude() >= current_feature.get_magnitude()
                        ? 1
                        : 0;

                remove_subsumed(clause, *child_node, depth + offset);
                if (child_node->children.empty() && child_node->clause_set.empty())
                    slated_for_removal.push_back(child_feature);
            }

        for (const auto &target: slated_for_removal)
            node.children.erase(target);
    }
}

void FVIKnowledgeBase::explore_and_remove_leaf(const Clause &incoming_clause, FVINode &node)
{
    auto next_clause_it = node.clause_set.begin();
    for (auto clause_it = next_clause_it; clause_it != node.clause_set.end(); clause_it = next_clause_it) {
        ++next_clause_it;
        const auto subsumes = incoming_clause.subsumes(**clause_it, unification_visitor);
        unification_visitor.reset_substitutions();
        if (subsumes) {
            LOG4CXX_INFO(kb_logger,
                    std::format("Orphaning clause {} from the KB as it is being subsumed by {}.", **clause_it,
                            incoming_clause));

            orphaned_clauses.insert(std::move(node.clause_set.extract(clause_it).value()));
            --total_clause_count;

            LOG4CXX_DEBUG(kb_logger,
                    std::format("The KB now contains {} active and {} orphaned clauses.", total_clause_count,
                            orphaned_clauses.size()));
        }
    }

    std::vector<Feature> slated_for_removal;
    for (const auto &[child_feature, child_node]: node.children) {
        explore_and_remove_leaf(incoming_clause, *child_node);
        if (child_node->children.empty() && child_node->clause_set.empty())
            slated_for_removal.push_back(child_feature);
    }

    for (const auto &target: slated_for_removal)
        node.children.erase(target);
}

unsigned int FVIKnowledgeBase::get_total_clause_count() const noexcept
{
    return total_clause_count;
}

bool FVIKnowledgeBase::is_orphaned(const Clause *clause) const
{
    return orphaned_clauses.contains(clause);
}

} // namespace optifol
