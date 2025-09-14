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

#include "../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../Visitors/RegularTargets/Unification/UnificationApplicationVisitor.hpp"
#include "../../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

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

bool Constant::accept(UnificationVisitor &unification_visitor, const IProcessedTerm &term) const
{
    return term.accept(unification_visitor, *this);
}

bool Constant::accept(UnificationVisitor &unification_visitor, const Variable &variable) const
{
    return unification_visitor.visit(variable, *this);
}

bool Constant::is_self_nested(const IProcessedTerm &search_term) const noexcept
{
    return false;
}

UnificationApplicationVisitor::VisitorReturn Constant::accept(
        const UnificationApplicationVisitor &unification_application_visitor) const
{
    return unification_application_visitor.visit(*this);
}

bool Constant::operator==(const IProcessedTerm &other) const noexcept
{
    const auto other_constant = dynamic_cast<const Constant *>(&other);
    if (other_constant == nullptr)
        // Other IProcessedTerm isn't a Constant.
        return false;

    return get_disambiguated_name() == other_constant->get_disambiguated_name();
}

} // namespace optifol
