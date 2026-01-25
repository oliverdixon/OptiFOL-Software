/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the FOL substitution-application visitor
 * @author Oliver Dixon
 * @date 2026-01-21
 * @version Development
 */

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
        const Unifier &substitutions, std::shared_ptr<SymbolRepository> symbol_repository) :
    substitutions(substitutions),
    existing_symbol_repository(std::move(symbol_repository)),
    new_symbol_repository(std::make_unique<SymbolRepository>())
{
}

const IProcessedTerm *UnificationApplicationVisitor::visit(const Constant &node) const
{
    return existing_symbol_repository->get_symbol_handle(node);
}

const IProcessedTerm *UnificationApplicationVisitor::visit(const Variable &node) const
{
    const auto it = substitutions.find(node);
    return it == substitutions.cend() ? existing_symbol_repository->get_symbol_handle(node) : it->second;
}

const IProcessedTerm *UnificationApplicationVisitor::visit(const Function &node) const
{
    auto transformed_arguments = apply_to_term_vector(node.observe_arguments());

    /*
     * If the unifier could be successfully applied component-wise to the arguments (indicated by the std::optional
     * containing a vector), create the applied Function symbol and add it to the SymbolRepository. Otherwise, provide
     * a handle to the original unmutated Function.
     */
    return transformed_arguments.has_value() ?
        existing_symbol_repository->add_symbol(std::make_unique<Function>(
            std::string(node.get_disambiguated_name()),
            std::move(*transformed_arguments))
        ) : existing_symbol_repository->get_symbol_handle(node);
}

const Literal *UnificationApplicationVisitor::visit(const Literal &node) const
{
    auto transformed_arguments = apply_to_term_vector(node.observe_arguments());
    return transformed_arguments.has_value() ?
        existing_symbol_repository->add_symbol<Literal>(std::make_unique<Literal>(
            std::string(node.get_name()),
            std::move(*transformed_arguments),
            !node.is_negative_polarity())
        ) : existing_symbol_repository->get_symbol_handle<Literal>(node);
}

void UnificationApplicationVisitor::discard_new_symbols()
{
    new_symbol_repository = std::make_unique<SymbolRepository>();
}

void UnificationApplicationVisitor::keep_new_symbols()
{
    existing_symbol_repository->inherit_repository(std::move(new_symbol_repository));
    discard_new_symbols();
}

std::optional<std::vector<const IProcessedTerm *>> UnificationApplicationVisitor::apply_to_term_vector(
        const std::vector<const IProcessedTerm *> &terms) const
{
    bool changed = false;
    std::vector<const IProcessedTerm *> transformed_arguments;

    transformed_arguments.reserve(terms.size());

    for (const auto &argument: terms) {
        const auto transformed_argument = argument->accept(*this);
        transformed_arguments.push_back(transformed_argument);

        if (transformed_argument != argument)
            changed = true;
    }

    if (!changed)
        /*
         * It's only worth reporting our transformed argument vector if a transformation occurred on at least one of the
         * arguments. Else it's just the orginal argument vector, and we can indicate this with an empty optional.
         */
        return std::nullopt;

    return transformed_arguments;
}

} // namespace optifol
