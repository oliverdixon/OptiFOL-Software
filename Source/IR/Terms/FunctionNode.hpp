//
// Created by owd on 19/11/24.
//

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
                                           std::vector<std::shared_ptr<ITermNode>> &&arguments) :
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

    [[nodiscard]] std::string get_disambiguated_name() const override
    {
        return to_string();
    }

    void accept(MutatingTermVisitorBase& visitor) override
    {
        visitor.visit(*this);
    }

    std::vector<std::shared_ptr<ITermNode>>& get_arguments()
    {
        return arguments;
    }

private:
    const std::string name;
    std::vector<std::shared_ptr<ITermNode>> arguments;
};

}

#endif //OPTIFOL_FUNCTIONNODE_HPP
