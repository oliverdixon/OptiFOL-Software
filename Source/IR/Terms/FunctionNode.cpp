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

#include "FunctionNode.hpp"

#include "../../Visitors/Terms/MutatingTermVisitorBase.hpp"

namespace optifol
{

FunctionNode::FunctionNode(std::string name, std::vector<std::unique_ptr<ITermNode>> &&arguments):
    name(std::move(name)),
    arguments(std::move(arguments))
{}

FunctionNode::FunctionNode(std::string name, const std::vector<std::unique_ptr<ITermNode>> &arguments) :
    name(std::move(name))
{
    this->arguments.reserve(arguments.size());
    for (const auto& argument : arguments)
        this->arguments.push_back(argument->clone());
}

std::string FunctionNode::to_string() const
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

std::unique_ptr<ITermNode> FunctionNode::clone() const
{
    std::vector<std::unique_ptr<ITermNode>> cloned_arguments;
    cloned_arguments.reserve(arguments.size());
    for (const auto& argument : arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<FunctionNode>(name, std::move(cloned_arguments));
}

std::string FunctionNode::get_disambiguated_name() const
{
    return name;
}

void FunctionNode::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

const std::vector<std::unique_ptr<ITermNode>> & FunctionNode::observe_arguments() const
{
    return arguments;
}

std::vector<std::unique_ptr<ITermNode>> &FunctionNode::observe_arguments()
{
    return arguments;
}

bool FunctionNode::unify_with_me(const FunctionNode &function)
{
    const auto argument_count = arguments.size();

    if (get_disambiguated_name() != function.get_disambiguated_name() || argument_count != function.arguments.size())
        return false;

    for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx)
        if (static_cast<UnifyCandidateBase *>(arguments[argument_idx].get())->
                unify_with_me(*function.arguments[argument_idx]) == false)
            return false;

    return true;
}

}
