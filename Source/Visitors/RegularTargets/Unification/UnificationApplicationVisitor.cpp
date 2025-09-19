/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 9/11/25.
//

#include "UnificationApplicationVisitor.hpp"

#include "../../../IR/SymbolRepository.hpp"
#include "../../../IR/Terms/Function.hpp"
#include "../../../IR/Sentences/Literal.hpp"

// ReSharper disable CppUnusedIncludeDirective - Full definitions required for transparent hashing of sub. map.
#include "../../../IR/Terms/Constant.hpp"
#include "../../../IR/Terms/Variable.hpp"
// ReSharper restore CppUnusedIncludeDirective

namespace optifol
{

UnificationApplicationVisitor::UnificationApplicationVisitor(
        const UnificationVisitor::SubstitutionMap &substitutions, std::shared_ptr<SymbolRepository> symbol_repository) :
    substitutions(substitutions),
    symbol_repository(std::move(symbol_repository))
{
}

UnificationApplicationVisitor::VisitorReturn UnificationApplicationVisitor::visit(const Constant &node)
{
    std::ignore = node;
    return nullptr;
}

UnificationApplicationVisitor::VisitorReturn UnificationApplicationVisitor::visit(const Variable &node) const
{
    const auto it = substitutions.find(node);
    return it == substitutions.cend() ? nullptr : it->second;
}

UnificationApplicationVisitor::VisitorReturn UnificationApplicationVisitor::visit(const Function &node) const
{
    auto transformed_arguments = apply_to_term_vector(node.observe_arguments());
    if (!transformed_arguments.has_value())
        return nullptr;

    return std::make_unique<Function>(std::string(node.get_disambiguated_name()), std::move(*transformed_arguments));
}

UnificationApplicationVisitor::LiteralReturn UnificationApplicationVisitor::visit(const Literal &literal) const
{
    auto transformed_arguments = apply_to_term_vector(literal.observe_arguments());
    if (!transformed_arguments.has_value())
        return nullptr;

    return std::make_unique<Literal>(std::string(literal.get_name()), std::move(*transformed_arguments));
}

std::optional<std::vector<const IProcessedTerm *>> UnificationApplicationVisitor::apply_to_term_vector(
        const std::vector<const IProcessedTerm *> &terms) const
{
    bool changed = false;
    std::vector<const IProcessedTerm *> transformed_arguments;

    transformed_arguments.reserve(terms.size());

    for (const auto &argument: terms) {
        auto transformed = argument->accept(*this);

        if (std::holds_alternative<std::unique_ptr<IProcessedTerm>>(transformed)) {

            auto subbed_argument = std::move(std::get<std::unique_ptr<IProcessedTerm>>(transformed));
            transformed_arguments.push_back(symbol_repository->add_symbol(std::move(subbed_argument)));
            changed = true;

        } else if (std::holds_alternative<const IProcessedTerm *>(transformed)) {

            const auto subbed_argument = std::get<const IProcessedTerm *>(transformed);
            if (subbed_argument == nullptr)
                transformed_arguments.push_back(argument);
            else {
                transformed_arguments.push_back(subbed_argument);
                changed = true;
            }

        }
    }

    if (!changed)
        return std::nullopt;

    return transformed_arguments;
}

} // namespace optifol
