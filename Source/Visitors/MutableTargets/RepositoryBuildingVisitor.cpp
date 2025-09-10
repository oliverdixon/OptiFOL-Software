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
#include <iostream>

#include "../../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../../IR/MutableVariants/Sentences/MutableIdentity.hpp"
#include "../../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../../IR/MutableVariants/Sentences/MutableQuantified.hpp"
#include "../../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../../IR/MutableVariants/Terms/MutableConstant.hpp"
#include "../../IR/MutableVariants/Terms/MutableFunction.hpp"
#include "../../IR/MutableVariants/Terms/MutableVariable.hpp"
#include "../../IR/Sentences/Identity.hpp"
#include "../../IR/Sentences/Literal.hpp"
#include "../../IR/Sentences/Quantified.hpp"
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

const Quantified *RepositoryBuildingVisitor::visit(MutableQuantified &node)
{
    const auto bound_term = node.take_bound_term();
    const auto repo_term = bound_term->accept(*this);

    const auto bound_sentence = node.take_sentence();
    const auto repo_sentence = bound_sentence->accept(*this);

    return symbol_repository->add_symbol<Quantified>(std::make_unique<Quantified>(
            node.get_quantifier_type(), repo_term, repo_sentence, !node.is_negative_polarity()));
}

const BinaryConnected *RepositoryBuildingVisitor::visit(MutableBinaryConnected &node)
{
    const auto bound_lhs = node.take_lhs_operand();
    const auto repo_lhs = bound_lhs->accept(*this);

    const auto bound_rhs = node.take_rhs_operand();
    const auto repo_rhs = bound_rhs->accept(*this);

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

    return symbol_repository->add_symbol<Literal>(std::make_unique<Literal>(
            std::string(node.get_name()), std::move(processed_terms), !node.is_negative_polarity()));
}

void RepositoryBuildingVisitor::visit(MutableSentenceRoot &node)
{
    assert(root == nullptr);
    const auto sentence = node.take_sentence();

    /*
     * TODO URGENT HERE OWD: the below accept can just produce a list of literals under recursive disjunction. We're in
     *  CNF by this point; an exception can be thrown if we encounter anything other than the expected form, as it's a
     *  BUG.
     */
    root = std::make_unique<SentenceRoot>(sentence->accept(*this), !node.is_negative_polarity());
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
