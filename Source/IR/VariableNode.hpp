//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_VARIABLENODE_HPP
#define OPTIFOL_VARIABLENODE_HPP

#include <optional>
#include "ITermNode.hpp"
#include "../Visitors/Terms/ITermVisitor.hpp"

namespace optifol
{

class VariableNode :
        public ITermNode
{
public:
    explicit VariableNode(std::string name) :
            name(std::move(name))
    {}

    explicit VariableNode(std::string name, const std::string& disambiguated_name) :
            name(std::move(name)),
            disambiguated_name(disambiguated_name)
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return name;
    }

    [[nodiscard]] std::string get_disambiguated_name() const override
    {
        return disambiguated_name.value_or(name);
    }

    void accept(ITermVisitor& visitor) override
    {
        visitor.visit(*this);
    }

private:
    const std::string name;
    std::optional<std::string> disambiguated_name;
};

}

#endif //OPTIFOL_VARIABLENODE_HPP
