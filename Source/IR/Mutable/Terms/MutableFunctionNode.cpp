/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Function Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableFunctionNode.hpp"

#include "../../../Visitors/Terms/MutatingTermVisitorBase.hpp"
#include "../../../Visitors/Unification/UnificationVisitor.hpp"

namespace optifol
{

MutableFunctionNode::MutableFunctionNode(std::string name, std::vector<std::unique_ptr<IMutableTermNode>> &&arguments) :
    name(std::move(name)), arguments(std::move(arguments))
{
}

MutableFunctionNode::MutableFunctionNode(std::string name, const std::vector<std::unique_ptr<IMutableTermNode>> &arguments) :
    name(std::move(name))
{
    this->arguments.reserve(arguments.size());
    for (const auto &argument: arguments)
        this->arguments.push_back(argument->clone());
}

std::string MutableFunctionNode::to_string() const
{
    std::string result = get_disambiguated_name() + '(';

    auto argument_count = arguments.size();

    for (const auto &arg: arguments) {
        result += arg->to_string();
        if (--argument_count > 0)
            result += ", ";
    }

    result += ')';
    return result;
}

std::unique_ptr<IMutableTermNode> MutableFunctionNode::clone() const
{
    std::vector<std::unique_ptr<IMutableTermNode>> cloned_arguments;
    cloned_arguments.reserve(arguments.size());
    for (const auto &argument: arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<MutableFunctionNode>(name, std::move(cloned_arguments));
}

std::string MutableFunctionNode::get_disambiguated_name() const
{
    return name;
}

void MutableFunctionNode::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

bool MutableFunctionNode::accept(UnificationVisitor &visitor, const IMutableTermNode &target) const
{
    return target.accept(visitor, *this);
}

bool MutableFunctionNode::accept(UnificationVisitor &visitor, const MutableFunctionNode &target) const
{
    return visitor.visit(*this, target);
}

const std::vector<std::unique_ptr<IMutableTermNode>> &MutableFunctionNode::observe_arguments() const
{
    return arguments;
}

std::vector<std::unique_ptr<IMutableTermNode>> &MutableFunctionNode::observe_arguments()
{
    return arguments;
}

std::ostream &MutableFunctionNode::serialise(std::ostream &ostream) const
{
    ostream << '$' << name << '(';

    if (arguments.empty() == false) {
        const auto argument_count = arguments.size() - 1;

        for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx) {
            arguments[argument_idx]->serialise(ostream);
            ostream << ", ";
        }

        arguments[argument_count]->serialise(ostream);
    }

    return ostream << ')';
}

} // namespace optifol
