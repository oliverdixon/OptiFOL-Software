/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Variable Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEVARIABLENODE_HPP
#define MUTABLEVARIABLENODE_HPP

#include <optional>

#include "../OwningBuildable.hpp"
#include "IMutableTermNode.hpp"

namespace optifol
{

class MutableVariableNode :
        public IMutableTermNode,
        public OwningBuildable<MutableVariableNode>
{
public:
    explicit MutableVariableNode(std::string name);

    explicit MutableVariableNode(std::string name, const std::string& disambiguated_name);

    [[nodiscard]] std::unique_ptr<IMutableTermNode> clone() const override;

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

    void accept(MutatingTermVisitorBase& visitor) override;

    bool accept(UnificationVisitor &visitor, const IMutableTermNode &target) const override;

    bool accept(UnificationVisitor &visitor, const MutableVariableNode &target) const override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    const std::string name;

    std::optional<std::string> disambiguated_name;
};

}

#endif
