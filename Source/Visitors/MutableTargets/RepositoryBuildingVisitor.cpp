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

#include <assert.h>
#include <iostream>

#include "../../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../../IR/MutableVariants/Sentences/MutableIdentity.hpp"
#include "../../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../../IR/MutableVariants/Sentences/MutableQuantified.hpp"
#include "../../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../../IR/MutableVariants/Terms/MutableVariable.hpp"
#include "../../IR/Sentences/Identity.hpp"
#include "../../IR/Sentences/Predicate.hpp"
#include "../../IR/Sentences/Quantified.hpp"
#include "../../IR/Sentences/SentenceRoot.hpp"
#include "../../IR/Terms/Variable.hpp"

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

const Quantified *RepositoryBuildingVisitor::visit(MutableQuantified &node)
{
    const auto bound_term = node.take_bound_term();
    const auto repo_term = bound_term->accept(*this);

    const auto bound_sentence = node.take_sentence();
    const auto repo_sentence = bound_sentence->accept(*this);

    return symbol_repository.add_symbol<Quantified>(std::make_unique<Quantified>(node.get_quantifier_type(), repo_term,
        repo_sentence, !node.is_negative_polarity()));
}

const BinaryConnected *RepositoryBuildingVisitor::visit(MutableBinaryConnected &node)
{
    const auto bound_lhs = node.take_lhs_operand();
    const auto repo_lhs = bound_lhs->accept(*this);

    const auto bound_rhs = node.take_rhs_operand();
    const auto repo_rhs = bound_rhs->accept(*this);

    return symbol_repository.add_symbol<BinaryConnected>(std::make_unique<BinaryConnected>(node.get_operator_type(),
        repo_lhs, repo_rhs));
}

const Identity *RepositoryBuildingVisitor::visit(MutableIdentity &node)
{
    const auto bound_lhs = node.take_lhs_operand();
    const auto repo_lhs = bound_lhs->accept(*this);

    const auto bound_rhs = node.take_rhs_operand();
    const auto repo_rhs = bound_rhs->accept(*this);

    return symbol_repository.add_symbol<Identity>(std::make_unique<Identity>(repo_lhs, repo_rhs));
}

const Predicate *RepositoryBuildingVisitor::visit(MutablePredicate &predicate)
{
    const auto &owned_arguments = predicate.observe_arguments();
    std::vector<const IProcessedTerm *> processed_arguments;
    processed_arguments.reserve(owned_arguments.size());

    for (const auto &argument: owned_arguments)
        processed_arguments.push_back(argument->accept(*this));

    return symbol_repository.add_symbol<Predicate>(std::make_unique<Predicate>(
            std::string(predicate.get_name()), std::move(processed_arguments), !predicate.is_negative_polarity()));
}

void RepositoryBuildingVisitor::visit(MutableSentenceRoot &node)
{
    assert(root == nullptr);
    const auto sentence = node.take_sentence();
    root = std::make_unique<SentenceRoot>(sentence->accept(*this), !node.is_negative_polarity());
}

std::unique_ptr<SentenceRoot> RepositoryBuildingVisitor::take_last_root() noexcept
{
    assert(root != nullptr);
    return std::move(root);
}

const Variable *RepositoryBuildingVisitor::visit(const MutableVariable &node) const
{
    return symbol_repository.add_symbol<Variable>(std::make_unique<Variable>(node.to_string(),
        std::string(node.get_disambiguated_name())));
}

} // namespace optifol
