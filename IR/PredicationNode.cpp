//
// Created by owd on 24/11/24.
//

#include "PredicationNode.hpp"
#include "../Visitors/VisitorBase.hpp"

namespace optifol
{

PredicationNode::PredicationNode(std::string name, std::vector<std::shared_ptr<ITermNode>> &&arguments) :
        name(std::move(name)),
        arguments(std::move(arguments))
{}

std::string PredicationNode::to_string() const
{
    std::string result = name + '(';

    auto argument_count = arguments.size();

    for (const auto &arg: arguments) {
        result += arg->to_string();
        if (--argument_count > 0)
            result += ", ";
    }

    result += ')';
    return result;
}

void PredicationNode::accept(VisitorBase &visitor)
{
    visitor.visit(*this);
}

}
