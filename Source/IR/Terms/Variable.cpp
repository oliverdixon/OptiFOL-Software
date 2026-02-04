/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Function IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#include "Variable.hpp"

#include "../../Inference/FVI/FeatureComponentBuilder.hpp"
#include "../../Visitors/RegularTargets/Unification/BidirectionalUnificationVisitor.hpp"

namespace optifol
{

Variable::Variable(std::string name) :
    name(std::move(name))
{
}

Variable::Variable(std::string name, std::string disambiguated_name) :
    name(std::move(name)),
    disambiguated_name(std::move(disambiguated_name))
{
}

std::string Variable::to_string() const
{
    return std::string(get_disambiguated_name());
}

std::string_view Variable::get_disambiguated_name() const
{
    if (disambiguated_name.has_value())
        return *disambiguated_name;

    return name;
}

std::string_view Variable::get_base_name() const noexcept
{
    return name;
}

bool Variable::operator==(const Variable &other) const
{
    return disambiguated_name == other.disambiguated_name;
}

bool Variable::accept(IObservingBinaryVisitor &binary_visitor, const IProcessedTerm &term) const
{
    return term.accept_reverse(binary_visitor, *this);
}

bool Variable::accept(IObservingBinaryVisitor &binary_visitor, const Constant &constant) const
{
    return binary_visitor.visit(*this, constant);
}

bool Variable::accept(IObservingBinaryVisitor &binary_visitor, const Function &function) const
{
    return binary_visitor.visit(*this, function);
}

bool Variable::accept(IObservingBinaryVisitor &binary_visitor, const Variable &variable) const
{
    return binary_visitor.visit(*this, variable);
}

bool Variable::accept_reverse(IObservingBinaryVisitor &binary_visitor, const Constant &constant) const
{
    return binary_visitor.visit(constant, *this);
}

bool Variable::accept_reverse(IObservingBinaryVisitor &binary_visitor, const Function &function) const
{
    return binary_visitor.visit(function, *this);
}

bool Variable::accept_reverse(IObservingBinaryVisitor &binary_visitor, const Variable &variable) const
{
    return binary_visitor.visit(variable, *this);
}

bool Variable::is_self_nested(const IProcessedTerm &search_term) const noexcept
{
    return search_term == *this;
}

const IProcessedTerm *Variable::accept(const UnificationApplicationVisitor &unification_application_visitor) const
{
    return unification_application_visitor.visit(*this);
}

void Variable::accept(const FeatureComponentBuilder &feature_builder) const
{
    feature_builder.visit(this);
}

bool Variable::operator==(const IProcessedTerm &other) const noexcept
{
    const auto other_variable = dynamic_cast<const Variable *>(&other);
    if (other_variable == nullptr)
        // Other IProcessedTerm isn't a Variable.
        return false;

    return get_disambiguated_name() == other_variable->get_disambiguated_name();
}

bool Variable::operator<(const IProcessedTerm &other) const noexcept
{
    const auto other_variable = dynamic_cast<const Variable *>(&other);
    if (other_variable == nullptr)
        // Other IProcessedTerm isn't a Variable.
        return false;

    return get_disambiguated_name() < other.get_disambiguated_name();
}

} // namespace optifol
