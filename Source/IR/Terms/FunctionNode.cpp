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

std::string FunctionNode::to_string() const
{
    std::string result = name + '(';

    auto argument_count = arguments.size();

    for (const auto &arg: arguments) {
        result += arg->get_disambiguated_name();
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
    return to_string();
}

void FunctionNode::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

std::vector<std::unique_ptr<ITermNode>> & FunctionNode::observe_arguments()
{
    return arguments;
}

}
