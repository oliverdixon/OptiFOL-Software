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

#include <cassert>

#include "../../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../../IR/MutableVariants/Sentences/MutableIdentity.hpp"
#include "../../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../../IR/MutableVariants/Terms/MutableConstant.hpp"
#include "../../IR/MutableVariants/Terms/MutableFunction.hpp"
#include "../../IR/MutableVariants/Terms/MutableVariable.hpp"
#include "../../IR/Sentences/Identity.hpp"
#include "../../IR/Sentences/Literal.hpp"
#include "../../IR/Sentences/SentenceRoot.hpp"
#include "../../IR/Terms/Constant.hpp"
#include "../../IR/Terms/Function.hpp"
#include "../../IR/Terms/Variable.hpp"

namespace optifol
{

const char *RepositoryBuildingVisitor::visitor_name = "Repository-Building Visitor";

RepositoryBuildingVisitor::RepositoryBuildingVisitor(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository))
{
}

std::string_view RepositoryBuildingVisitor::get_visitor_name()
{
    return visitor_name;
}

const IProcessedSentence *RepositoryBuildingVisitor::visit(const MutableQuantified &node)
{
    std::ignore = node;
    assert(false);
    // ReSharper disable once CppDFAUnreachableCode - Not unreachable if assertions disabled.
    return nullptr;
}

const BinaryConnected *RepositoryBuildingVisitor::visit(MutableBinaryConnected &node)
{
    const auto operator_type = node.get_operator_type();
    assert(operator_type == BinaryOperatorTypes::Conjunction || operator_type == BinaryOperatorTypes::Disjunction);

    /*
     * The "manage clause" flag indicates that this function is responsible for committing and resetting the
     * working clause member function, and that its callees are exclusively allowed to insert literals into the working
     * clause. Note that this should be set for any conjunctive node, and if an existing non-empty working clause is
     * present, it may be safely assumed to be fully populated and committed to the sentence root.
     */
    const bool managed_clause = operator_type == BinaryOperatorTypes::Conjunction;

    // Create a fresh clause for the LHS operand, committing a previously populated clause if necessary.
    if (managed_clause && !working_clause.empty()) {
        root->commit_clause(working_clause);
        working_clause.clear();
    }

    const auto bound_lhs = node.take_lhs_operand();
    const auto repo_lhs = bound_lhs->accept(*this);

    /*
     * If the LHS recursion produced any literals under disjunction, commit the set to the sentence root, and create a
     * fresh clause for the RHS operand.
     */
    if (managed_clause && !working_clause.empty()) {
        root->commit_clause(working_clause);
        working_clause.clear();
    }

    const auto bound_rhs = node.take_rhs_operand();
    const auto repo_rhs = bound_rhs->accept(*this);

    // Likewise, commit any disjunctive literals produced by the RHS recursion to the sentence root.
    if (managed_clause && !working_clause.empty()) {
        root->commit_clause(working_clause);
        working_clause.clear();
    }

    return symbol_repository->add_symbol<BinaryConnected>(
            std::make_unique<BinaryConnected>(node.get_operator_type(), repo_lhs, repo_rhs));
}

const Identity *RepositoryBuildingVisitor::visit(MutableIdentity &node)
{
    const auto bound_lhs = node.take_lhs_operand();
    const auto repo_lhs = bound_lhs->accept(*this);

    const auto bound_rhs = node.take_rhs_operand();
    const auto repo_rhs = bound_rhs->accept(*this);

    return symbol_repository->add_symbol<Identity>(std::make_unique<Identity>(repo_lhs, repo_rhs));
}

const Literal *RepositoryBuildingVisitor::visit(MutablePredicate &node)
{
    const auto &owned_terms = node.observe_arguments();
    std::vector<const IProcessedTerm *> processed_terms;
    processed_terms.reserve(owned_terms.size());

    for (const auto &term: owned_terms)
        processed_terms.push_back(term->accept(*this));

    // Create the Literal symbol and append to the working clause.
    const auto new_symbol = symbol_repository->add_symbol<Literal>(std::make_unique<Literal>(
            std::string(node.get_name()), std::move(processed_terms), !node.is_negative_polarity()));
    working_clause.push_back(new_symbol);
    return new_symbol;
}

void RepositoryBuildingVisitor::visit(MutableSentenceRoot &node)
{
    assert(root == nullptr);
    root = std::make_unique<SentenceRoot>();
    const auto sentence = node.take_sentence();
    sentence->accept(*this);

    if (!working_clause.empty()) {
        root->commit_clause(working_clause);
        working_clause.clear();
    }
}

std::unique_ptr<SentenceRoot> RepositoryBuildingVisitor::take_last_root() noexcept
{
    assert(root != nullptr);
    return std::move(root);
}

const Variable *RepositoryBuildingVisitor::visit(const MutableVariable &node) const
{
    return symbol_repository->add_symbol<Variable>(
            std::make_unique<Variable>(node.to_string(), std::string(node.get_disambiguated_name())));
}

const Function *RepositoryBuildingVisitor::visit(MutableFunction &node)
{
    const auto &owned_terms = node.observe_arguments();
    std::vector<const IProcessedTerm *> processed_terms;
    processed_terms.reserve(owned_terms.size());

    for (const auto &term: owned_terms)
        processed_terms.push_back(term->accept(*this));

    return symbol_repository->add_symbol<Function>(
            std::make_unique<Function>(std::string(node.get_disambiguated_name()), std::move(processed_terms)));
}

const Constant *RepositoryBuildingVisitor::visit(const MutableConstant &node) const
{
    return symbol_repository->add_symbol<Constant>(std::make_unique<Constant>(node.to_string()));
}

} // namespace optifol
