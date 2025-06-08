/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Constant Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef CONSTANTNODE_HPP
#define CONSTANTNODE_HPP

#include "../Support/Buildable.hpp"
#include "ITermNode.hpp"

namespace optifol
{

class ConstantNode :
        public ITermNode,
        public Buildable<ConstantNode>
{
public:
    [[maybe_unused]] explicit ConstantNode(std::string name);

    [[nodiscard]] std::unique_ptr<ITermNode> clone() const override;

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

    void accept(MutatingTermVisitorBase& visitor) override;

private:
    const std::string name;
};

}

#endif
