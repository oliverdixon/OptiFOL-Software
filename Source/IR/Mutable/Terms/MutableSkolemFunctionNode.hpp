/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Skolem Function Term IR node
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef MUTABLESKOLEMFUNCTIONNODE_HPP
#define MUTABLESKOLEMFUNCTIONNODE_HPP

#include <vector>

#include "MutableFunctionNode.hpp"

namespace optifol
{

class MutableVariableNode;

class MutableSkolemFunctionNode :
        public MutableFunctionNode
{
public:
    [[maybe_unused]] explicit MutableSkolemFunctionNode(std::string name,
        std::vector<std::unique_ptr<IMutableTermNode>> &&quantified_variables = {});

    [[maybe_unused]] explicit MutableSkolemFunctionNode(std::string name,
        const std::vector<std::unique_ptr<IMutableTermNode>> &quantified_variables);

    [[nodiscard]] std::unique_ptr<IMutableTermNode> clone() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;
};

}

#endif
