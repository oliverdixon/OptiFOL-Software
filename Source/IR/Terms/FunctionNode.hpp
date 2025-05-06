/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Function Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef FUNCTIONNODE_HPP
#define FUNCTIONNODE_HPP

#include <memory>
#include <vector>

#include "ITermNode.hpp"

namespace optifol
{

class FunctionNode :
        public ITermNode
{
public:
    [[maybe_unused]] explicit FunctionNode(std::string name, std::vector<std::unique_ptr<ITermNode>> &&arguments);

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::unique_ptr<ITermNode> clone() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

    void accept(MutatingTermVisitorBase& visitor) override;

    std::vector<std::unique_ptr<ITermNode>>& observe_arguments();

private:
    const std::string name;

    std::vector<std::unique_ptr<ITermNode>> arguments;
};

}

#endif
