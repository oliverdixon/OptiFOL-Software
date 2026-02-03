/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the FOL binary-unification visitor (unidirectional)
 * @author Oliver Dixon
 * @date 2025-06-08
 * @version Development
 */

#include "UnificationVisitor.hpp"

#include <algorithm>
#include <ranges>

#include "../Exceptions/SemanticException.hpp"
#include "../../../IR/SymbolRepository.hpp"
#include "../../../IR/Sentences/Literal.hpp"
#include "../../../IR/Terms/Constant.hpp"
#include "../../../IR/Terms/Function.hpp"

namespace optifol
{

UnificationVisitor::UnificationVisitor(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository)),
    substitutions(std::make_shared<Unifier>())
{
}

bool UnificationVisitor::visit(const Literal &predicate_gen, const Literal &predicate_inst)
{
    if (predicate_gen.is_negative_polarity() != predicate_inst.is_negative_polarity())
        return false;

    if (predicate_gen.get_name() != predicate_inst.get_name())
        return false;

    const auto& gen_args = predicate_gen.observe_arguments();
    const auto& inst_args = predicate_inst.observe_arguments();

    if (gen_args.size() != inst_args.size())
        return false;

    return std::ranges::all_of(std::ranges::views::zip(gen_args, inst_args),
        [this](const auto& arg_pair) { return std::get<0>(arg_pair)->accept(*this, *std::get<1>(arg_pair)); });
}

bool UnificationVisitor::visit(const Constant &constant_gen, const Constant &constant_inst)
{
    return constant_gen.operator==(constant_inst);
}

bool UnificationVisitor::visit(const Constant &constant_gen, const Function &function_inst)
{
    std::ignore = constant_gen;
    std::ignore = function_inst;
    
    return false;
}

bool UnificationVisitor::visit(const Constant &constant_gen, const Variable &variable_inst)
{
    std::ignore = constant_gen;
    std::ignore = variable_inst;

    return false;
}

bool UnificationVisitor::visit(const Variable &variable_gen, const Constant &constant_inst)
{
    return variable_generic(variable_gen, constant_inst);
}

bool UnificationVisitor::visit(const Variable &variable_gen, const Function &function_inst)
{
    return variable_generic(variable_gen, function_inst);
}

bool UnificationVisitor::visit(const Variable &variable_gen, const Variable &variable_inst)
{
    return variable_generic(variable_gen, variable_inst);
}

bool UnificationVisitor::visit(const Function &function_gen, const Constant &constant_inst)
{
    std::ignore = function_gen;
    std::ignore = constant_inst;

    return false;
}

bool UnificationVisitor::visit(const Function &function_gen, const Variable &variable_inst)
{
    std::ignore = function_gen;
    std::ignore = variable_inst;

    return false;
}

bool UnificationVisitor::visit(const Function &function_gen, const Function &function_inst)
{
    if (function_gen.get_disambiguated_name() != function_inst.get_disambiguated_name())
        return false;

    const auto &gen_args = function_gen.observe_arguments();
    const auto &inst_args = function_inst.observe_arguments();

    if (gen_args.size() != inst_args.size())
        return false;

    return std::ranges::all_of(std::ranges::views::zip(gen_args, inst_args),
        [this](const auto& arg_pair) { return std::get<0>(arg_pair)->accept(*this, *std::get<1>(arg_pair)); });
}

const Unifier *UnificationVisitor::observe_substitutions() const noexcept
{
    return substitutions.get();
}

std::shared_ptr<Unifier> UnificationVisitor::share_substitutions() const noexcept
{
    return substitutions;
}

void UnificationVisitor::reset_substitutions() const noexcept
{
    substitutions->unifier.clear();
}

void UnificationVisitor::register_substitution(const Variable &bound_key, const IProcessedTerm &bound_value) const
{
    const auto variable_repo_ptr = symbol_repository->get_symbol_handle(bound_key);
    const auto bound_repo_ptr = symbol_repository->get_symbol_handle(bound_value);

    if (variable_repo_ptr == nullptr || bound_repo_ptr == nullptr)
        throw SemanticException("Attempted to register substitution for " + bound_key.to_string() + " but the "
            "Repository is incomplete.");

    substitutions->unifier.emplace(variable_repo_ptr, bound_repo_ptr);
}

bool UnificationVisitor::variable_generic(const Variable &variable_gen, const IProcessedTerm &generic_term_inst)
{
    const auto it = substitutions->unifier.find(variable_gen);
    if (it != substitutions->unifier.end())
        return variable_gen.operator==(*it->first);

    register_substitution(variable_gen, generic_term_inst);
    return true;
}

} // namespace optifol
