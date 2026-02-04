/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Constant Term IR node
 * @author Oliver Dixon
 * @date 2025-06-18
 * @version Development
 */

#include "Constant.hpp"

#include "../../Inference/FVI/FeatureComponentBuilder.hpp"
#include "../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../Visitors/RegularTargets/Unification/BidirectionalUnificationVisitor.hpp"

namespace optifol
{

Constant::Constant(std::string name) :
    name(std::move(name))
{
}

std::string Constant::to_string() const
{
    return name;
}

std::string_view Constant::get_disambiguated_name() const
{
    return name;
}

bool Constant::accept(IObservingBinaryVisitor &binary_visitor, const IProcessedTerm &term) const
{
    return term.accept_reverse(binary_visitor, *this);
}

bool Constant::accept(IObservingBinaryVisitor &binary_visitor, const Constant &constant) const
{
    return binary_visitor.visit(*this, constant);
}

bool Constant::accept(IObservingBinaryVisitor &binary_visitor, const Function &function) const
{
    return binary_visitor.visit(*this, function);
}

bool Constant::accept(IObservingBinaryVisitor &binary_visitor, const Variable &variable) const
{
    return binary_visitor.visit(*this, variable);
}

bool Constant::accept_reverse(IObservingBinaryVisitor &binary_visitor, const Constant &constant) const
{
    return binary_visitor.visit(constant, *this);
}

bool Constant::accept_reverse(IObservingBinaryVisitor &binary_visitor, const Function &function) const
{
    return binary_visitor.visit(function, *this);
}

bool Constant::accept_reverse(IObservingBinaryVisitor &binary_visitor, const Variable &variable) const
{
    return binary_visitor.visit(variable, *this);
}

bool Constant::is_self_nested(const IProcessedTerm &search_term) const noexcept
{
    return false;
}

const IProcessedTerm *Constant::accept(const UnificationApplicationVisitor &unification_application_visitor) const
{
    return unification_application_visitor.visit(*this);
}

void Constant::accept(const FeatureComponentBuilder &feature_builder) const
{
    feature_builder.visit(this);
}

bool Constant::operator==(const IProcessedTerm &other) const noexcept
{
    const auto other_constant = dynamic_cast<const Constant *>(&other);
    if (other_constant == nullptr)
        // Other IProcessedTerm isn't a Constant.
        return false;

    return get_disambiguated_name() == other_constant->get_disambiguated_name();
}

bool Constant::operator<(const IProcessedTerm &other) const noexcept
{
    const auto other_constant = dynamic_cast<const Constant *>(&other);
    if (other_constant == nullptr)
        // Other IProcessedTerm isn't a Constant.
            return false;

    return get_disambiguated_name() < other_constant->get_disambiguated_name();
}

} // namespace optifol
