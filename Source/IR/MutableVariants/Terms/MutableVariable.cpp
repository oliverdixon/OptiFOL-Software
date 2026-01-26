/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Variable Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableVariable.hpp"

#include "../../../Visitors/MutableTargets/Observers/IObservingNodeVisitor.hpp"
#include "../../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../../Visitors/MutableTargets/Terms/MutatingTermVisitorBase.hpp"
#include "../../Terms/Variable.hpp"
#include "MutableFunction.hpp"

namespace optifol
{

MutableVariable::MutableVariable(std::string name) :
    name(std::move(name))
{
}

MutableVariable::MutableVariable(std::string name, const std::string &disambiguated_name) :
    name(std::move(name)),
    disambiguated_name(disambiguated_name)
{
}

std::unique_ptr<IMutableTerm> MutableVariable::clone() const
{
    if (disambiguated_name.has_value())
        return std::make_unique<MutableVariable>(name, *disambiguated_name);

    return std::make_unique<MutableVariable>(name);
}

void MutableVariable::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

std::string MutableVariable::to_string() const
{
    return std::string(get_disambiguated_name());
}

std::string_view MutableVariable::get_disambiguated_name() const
{
    if (disambiguated_name.has_value())
        return *disambiguated_name;

    return name;
}

std::string_view MutableVariable::get_base_name() const noexcept
{
    return name;
}

const IProcessedTerm *MutableVariable::accept(RepositoryBuildingVisitor &visitor)
{
    return visitor.visit(*this);
}

void MutableVariable::accept(IObservingNodeVisitor &visitor) const
{
    visitor.visit(*this);
}

bool MutableVariable::operator==(const IMutableTerm &other) const noexcept
{
    const auto other_variable = dynamic_cast<const MutableVariable *>(&other);
    if (other_variable == nullptr)
        // Other IMutableTerm isn't a MutableVariable.
        return false;

    return get_disambiguated_name() == other_variable->get_disambiguated_name();
}

} // namespace optifol
