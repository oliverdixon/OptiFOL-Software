/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Skolem Function Term IR node
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef SKOLEMFUNCTIONNODE_HPP
#define SKOLEMFUNCTIONNODE_HPP

#include <vector>

#include "FunctionNode.hpp"

namespace optifol
{

class VariableNode;

class SkolemFunctionNode :
        public FunctionNode
{
public:
    [[maybe_unused]] explicit SkolemFunctionNode(std::string name,
        std::vector<std::unique_ptr<ITermNode>> &&quantified_variables);

    [[maybe_unused]] explicit SkolemFunctionNode(std::string name,
        const std::vector<std::unique_ptr<ITermNode>> &quantified_variables);

    [[nodiscard]] std::unique_ptr<ITermNode> clone() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;
};

}

#endif
