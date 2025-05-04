/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_FUNCTIONNODE_HPP
#define OPTIFOL_FUNCTIONNODE_HPP

#include <memory>
#include <utility>
#include <vector>

#include "ITermNode.hpp"
#include "../../Visitors/Terms/MutatingTermVisitorBase.hpp"

namespace optifol
{

class FunctionNode :
        public ITermNode
{
public:
    [[maybe_unused]] explicit FunctionNode(std::string name,
                                           std::vector<std::unique_ptr<ITermNode>> &&arguments) :
            name(std::move(name)), arguments(std::move(arguments))
    {}

    [[nodiscard]] std::string to_string() const override
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

    [[nodiscard]] std::unique_ptr<ITermNode> clone() const override
    {
        std::vector<std::unique_ptr<ITermNode>> cloned_arguments;
        cloned_arguments.reserve(arguments.size());
        for (const auto& argument : arguments)
            cloned_arguments.push_back(argument->clone());

        return std::make_unique<FunctionNode>(name, std::move(cloned_arguments));
    }

    [[nodiscard]] std::string get_disambiguated_name() const override
    {
        return to_string();
    }

    void accept(MutatingTermVisitorBase& visitor) override
    {
        visitor.visit(*this);
    }

    std::vector<std::unique_ptr<ITermNode>>& observe_arguments()
    {
        return arguments;
    }

private:
    const std::string name;
    std::vector<std::unique_ptr<ITermNode>> arguments;
};

}

#endif
