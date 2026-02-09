/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Feature Vector Indexing Clause knowledge base
 * @author Oliver Dixon
 * @date 2026-02-09
 * @version Development
 */

#ifndef OPTIFOL_FVIKNOWLEDGEBASE_HPP
#define OPTIFOL_FVIKNOWLEDGEBASE_HPP

#include <functional>
#include <generator>
#include <map>
#include <memory>
#include <vector>
#include <log4cxx/logger.h>

#include "../Optifol.hpp"
#include "../IR/Sentences/Clause.hpp"
#include "../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"
#include "Feature.hpp"

namespace optifol
{

class SymbolRepository;

class FVIKnowledgeBase
{
public:
    explicit FVIKnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository);

    FVIKnowledgeBase(const FVIKnowledgeBase& other_kb);
    FVIKnowledgeBase& operator=(FVIKnowledgeBase& other_kb) = delete;

    FVIKnowledgeBase(FVIKnowledgeBase&& old_kb) noexcept;
    FVIKnowledgeBase& operator=(FVIKnowledgeBase&& old_kb) noexcept;

    /**
     * @brief Introduces (and transfers ownership of) a new Clause into the knowledge base.
     * @details This function introduces a Clause to the KB on the condition that it is not subsumed by the existing KB.
     *  Furthermore, if its introduction causes clauses to be subsumed, those latter clauses are removed.
     * @param clause The Clause to introduce.
     * @param rejection_handler The callback to which ownership should be handed back if the knowledge base refuses the
     *  clause; this happens when the clause would be immediately subsumed by the KB.
     * @return An iterator to the inserted clause, or an <code>end</code> if refused, and a boolean flag indicating
     *  acceptance of the Clause by the KB.
     */
    std::pair<UniqueUnorderedSet<Clause>::iterator, bool> add_clause(std::unique_ptr<Clause> &&clause,
            const std::optional<std::function<void(std::unique_ptr<Clause> &&)>> &rejection_handler = {});

    [[nodiscard]] std::generator<const Clause *> flatten() const;

    [[nodiscard]] unsigned int get_total_clause_count() const noexcept;

    [[nodiscard]] bool is_orphaned(const Clause *clause) const;

private:
    struct FVINode
    {
        FVINode() = default;
        FVINode(const FVINode& src_node);
        FVINode& operator=(const FVINode& src_node);

        std::map<Feature, std::unique_ptr<FVINode>> children;
        UniqueUnorderedSet<Clause> clause_set;
    };

    struct OrphanHasher
    {
        using is_transparent = void;

        [[nodiscard]] std::size_t operator()(const Clause * const ptr) const noexcept
        {
            return reinterpret_cast<std::size_t>(ptr);
        }

        [[nodiscard]] std::size_t operator()(const std::unique_ptr<Clause>& unq) const noexcept
        {
            return reinterpret_cast<std::size_t>(unq.get());
        }
    };

    struct OrphanEquality
    {
        using is_transparent = void;

        [[nodiscard]] bool operator()(const Clause * const lhs_ptr, const Clause * const rhs_ptr) const noexcept
        {
            return lhs_ptr == rhs_ptr;
        }

        [[nodiscard]] bool operator()(const Clause * const lhs_ptr, const std::unique_ptr<Clause>& rhs_unq) const
            noexcept
        {
            return lhs_ptr == rhs_unq.get();
        }

        [[nodiscard]] bool operator()(const std::unique_ptr<Clause>& lhs_unq, const Clause * const rhs_ptr) const
            noexcept
        {
            return lhs_unq.get() == rhs_ptr;
        }

        [[nodiscard]] bool operator()(const std::unique_ptr<Clause>& lhs_unq, const std::unique_ptr<Clause>& rhs_unq)
            const noexcept
        {
            return lhs_unq.get() == rhs_unq.get();
        }
    };

    static constexpr auto unwrap_clause = std::views::transform(
        [](const std::unique_ptr<Clause>& clause){ return clause.get(); });

    static const log4cxx::LoggerPtr kb_logger;

    std::pair<UniqueUnorderedSet<Clause>::iterator, bool> insert_clause(std::unique_ptr<Clause> &&clause,
            const std::optional<std::function<void(std::unique_ptr<Clause> &&)>> &rejection_handler);

    void get_subsuming(const Clause &clause, const FVINode &node, unsigned int depth,
            std::vector<const Clause *> &subsuming_clauses) const;

    void get_subsumed(const Clause &clause, const FVINode &node, unsigned int depth,
            std::vector<const Clause *> &subsumed_clauses) const;

    void explore_leaf(const Clause& clause, const FVINode& node, std::vector<const Clause *>& subsumed_clauses) const;

    void remove_subsumed(const Clause &clause, FVINode &node, unsigned int depth);

    void explore_and_remove_leaf(const Clause &incoming_clause, FVINode &node);

    FVINode root;
    std::shared_ptr<SymbolRepository> symbol_repository;
    unsigned int total_clause_count = 0;

    std::unordered_set<std::unique_ptr<Clause>, OrphanHasher, OrphanEquality> orphaned_clauses;
    mutable UnificationVisitor unification_visitor;
};

} // namespace optifol

#endif // OPTIFOL_FVIKNOWLEDGEBASE_HPP
