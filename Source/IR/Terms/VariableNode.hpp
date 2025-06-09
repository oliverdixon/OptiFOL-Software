/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Variable Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef VARIABLENODE_HPP
#define VARIABLENODE_HPP

#include <optional>

#include "../Support/Buildable.hpp"
#include "ITermNode.hpp"

namespace optifol
{

class VariableNode :
        public ITermNode,
        public Buildable<VariableNode>
{
public:
    explicit VariableNode(std::string name);

    explicit VariableNode(std::string name, const std::string& disambiguated_name);

    [[nodiscard]] std::unique_ptr<ITermNode> clone() const override;

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

    void accept(MutatingTermVisitorBase& visitor) override;

    bool accept(UnificationVisitor &visitor, const ITermNode &target) const override;

    bool accept(UnificationVisitor &visitor, const VariableNode &target) const override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    const std::string name;

    std::optional<std::string> disambiguated_name;
};

}

#endif
