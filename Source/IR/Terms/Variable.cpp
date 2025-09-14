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

#include "../../Visitors/RegularTargets/Unification/UnificationApplicationVisitor.hpp"
#include "../../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

Variable::Variable(std::string name) :
    name(std::move(name))
{
}

Variable::Variable(std::string name, const std::string &disambiguated_name) :
    name(std::move(name)),
    disambiguated_name(disambiguated_name)
{
}

std::string Variable::to_string() const
{
    return name;
}

std::string_view Variable::get_disambiguated_name() const
{
    if (disambiguated_name.has_value())
        return *disambiguated_name;

    return name;
}

bool Variable::operator==(const Variable &other) const
{
    return disambiguated_name == other.disambiguated_name;
}

bool Variable::accept(UnificationVisitor &unification_visitor, const IProcessedTerm &term) const
{
    return term.accept(unification_visitor, *this);
}

bool Variable::accept(UnificationVisitor &unification_visitor, const Constant &constant) const
{
    return unification_visitor.visit(*this, constant);
}

bool Variable::accept(UnificationVisitor &unification_visitor, const Function &function) const
{
    return unification_visitor.visit(*this, function);
}

bool Variable::accept(UnificationVisitor &unification_visitor, const Variable &variable) const
{
    return unification_visitor.visit(*this, variable);
}

bool Variable::is_self_nested(const IProcessedTerm &search_term) const noexcept
{
    return search_term == *this;
}

UnificationApplicationVisitor::VisitorReturn Variable::accept(
        const UnificationApplicationVisitor &unification_application_visitor) const
{
    return unification_application_visitor.visit(*this);
}

bool Variable::operator==(const IProcessedTerm &other) const noexcept
{
    const auto other_variable = dynamic_cast<const Variable *>(&other);
    if (other_variable == nullptr)
        // Other IProcessedTerm isn't a Variable.
        return false;

    return get_disambiguated_name() == other_variable->get_disambiguated_name();
}

} // namespace optifol
