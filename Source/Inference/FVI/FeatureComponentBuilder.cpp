/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/30/26.
//

#include "FeatureComponentBuilder.hpp"

#include "../../IR/Sentences/Clause.hpp"
#include "../../IR/Terms/Function.hpp"
#include "../../IR/Terms/SkolemFunction.hpp"

namespace optifol
{

FeatureComponentBuilder::FeatureComponentBuilder(std::add_lvalue_reference_t<OCMapType> destination_oc,
        const bool is_positive) :
    is_positive(is_positive),
    destination_oc(destination_oc)
{
}

void FeatureComponentBuilder::visit(const Literal *literal) const
{
    mark_occurrence(literal);

    const auto& arguments = literal->observe_arguments();
    for (const auto argument : arguments)
        argument->accept(*this);
}

void FeatureComponentBuilder::visit(const Function *function) const
{
    mark_occurrence(function);

    const auto& arguments = function->observe_arguments();
    for (const auto argument : arguments)
        argument->accept(*this);
}

void FeatureComponentBuilder::visit(const SkolemFunction *skolem_function) const
{
    mark_occurrence(skolem_function);

    const auto& arguments = skolem_function->observe_arguments();
    for (const auto argument : arguments)
        argument->accept(*this);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void FeatureComponentBuilder::visit(const Variable *variable) const // NOLINT(*-convert-member-functions-to-static)
{
    std::ignore = variable;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void FeatureComponentBuilder::visit(const Constant *constant) const // NOLINT(*-convert-member-functions-to-static)
{
    std::ignore = constant;
}

void FeatureComponentBuilder::mark_occurrence(const Literal * const literal) const
{
    const auto it = destination_oc.find(literal);
    if (it == destination_oc.cend())
        destination_oc.emplace(OccurrenceCountFeature(literal, is_positive), 1);
    else
        ++it->second;
}

void FeatureComponentBuilder::mark_occurrence(const IProcessedTerm *term) const
{
    const auto it = destination_oc.find(term);
    if (it == destination_oc.cend())
        destination_oc.emplace(OccurrenceCountFeature(term, is_positive), 1);
    else
        ++it->second;
}

} // namespace optifol
