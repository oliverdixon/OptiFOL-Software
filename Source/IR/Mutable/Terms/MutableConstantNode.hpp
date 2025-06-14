/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Constant Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLECONSTANTNODE_HPP
#define MUTABLECONSTANTNODE_HPP

#include "../OwningBuildable.hpp"
#include "IMutableTermNode.hpp"

namespace optifol
{

class MutableConstantNode :
        public IMutableTermNode,
        public OwningBuildable<MutableConstantNode>
{
public:
    [[maybe_unused]] explicit MutableConstantNode(std::string name);

    [[nodiscard]] std::unique_ptr<IMutableTermNode> clone() const override;

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

    void accept(MutatingTermVisitorBase& visitor) override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    const std::string name;
};

}

#endif
