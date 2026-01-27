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

#include <algorithm>
#include <ranges>

#include "../../CompositeSerialisationHelpers.hpp"
#include "../../Visitors/RegularTargets/Unification/UnificationApplicationVisitor.hpp"
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
    return std::ranges::any_of(arguments, [&search_term](const IProcessedTerm * const argument)
        { return argument->is_self_nested(search_term); });
}

const IProcessedTerm *Function::accept(const UnificationApplicationVisitor &unification_application_visitor) const
{
    return unification_application_visitor.visit(*this);
}

bool Function::operator==(const IProcessedTerm &other) const noexcept
{
    const auto other_function = dynamic_cast<const Function *>(&other);
    if (other_function == nullptr)
        // Other IProcessedTerm isn't a MutableFunction.
        return false;

    if (get_disambiguated_name() != other_function->get_disambiguated_name())
        // Different superficial names.
        return false;

    const auto argument_count = arguments.size();
    if (argument_count != other_function->arguments.size())
        // Different number of arguments.
        return false;

    for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx) {
        const auto &lhs_arg_ptr = arguments[argument_idx];
        const auto &rhs_arg_ptr = other_function->arguments[argument_idx];

        if (lhs_arg_ptr == nullptr) {
            if (rhs_arg_ptr != nullptr)
                return false;
        } else if (rhs_arg_ptr == nullptr)
            return false;
        else if (*lhs_arg_ptr != *rhs_arg_ptr)
            return false;
    }

    return true;
}
bool Function::operator<(const IProcessedTerm &other) const noexcept
{
    const auto other_function = dynamic_cast<const Function *>(&other);
    if (other_function == nullptr)
        // Other IProcessedTerm isn't a MutableFunction.
        return false;

    return std::ranges::any_of(std::views::zip(arguments, other_function->arguments), [](const auto& pair)
    {
        return *std::get<0>(pair) < *std::get<1>(pair);
    });
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
