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

#include "Function.hpp"

#include "../../CompositeSerialisationHelpers.hpp"
#include "../../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

Function::Function(std::string name, const std::initializer_list<const IProcessedTerm *> arguments) :
    name(std::move(name)),
    arguments(arguments)
{
}

Function::Function(std::string name, std::vector<const IProcessedTerm *> &&arguments) :
    name(std::move(name)),
    arguments(std::move(arguments))
{
}

const std::vector<const IProcessedTerm *> &Function::observe_arguments() const noexcept
{
    return arguments;
}

bool Function::accept(UnificationVisitor &unification_visitor, const IProcessedTerm &term) const
{
    return term.accept(unification_visitor, *this);
}

bool Function::accept(UnificationVisitor &unification_visitor, const Function &function) const
{
    return unification_visitor.visit(*this, function);
}

bool Function::accept(UnificationVisitor &unification_visitor, const Variable &variable) const
{
    return unification_visitor.visit(variable, *this);
}

bool Function::is_self_nested(const IProcessedTerm &search_term) const noexcept
{
    for (const auto argument : arguments)
        if (argument->is_self_nested(search_term))
            return true;

    return false;
}

std::string Function::to_string() const
{
    return CompositeSerialisationHelpers::string_serialise(name, arguments.cbegin(), arguments.cend());
}

std::string_view Function::get_disambiguated_name() const
{
    return name;
}

std::ostream &Function::serialise(std::ostream &ostream) const
{
    return CompositeSerialisationHelpers::stream_serialise(ostream, name, arguments.cbegin(), arguments.cend());
}

std::size_t Function::hash() const noexcept
{
    return composite_hash(name, arguments.cbegin(), arguments.cend());
}

} // namespace optifol
