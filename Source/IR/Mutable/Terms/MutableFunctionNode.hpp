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

#ifndef MUTABLEFUNCTIONNODE_HPP
#define MUTABLEFUNCTIONNODE_HPP

#include <memory>
#include <vector>

#include "../OwningBuildable.hpp"
#include "IMutableTermNode.hpp"

namespace optifol
{

class MutableFunctionNode :
        public IMutableTermNode,
        public OwningBuildable<MutableFunctionNode>
{
public:
    [[maybe_unused]] explicit MutableFunctionNode(std::string name, std::vector<std::unique_ptr<IMutableTermNode>> &&arguments = {});

    [[maybe_unused]] explicit MutableFunctionNode(std::string name, const std::vector<std::unique_ptr<IMutableTermNode>> &arguments);

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::unique_ptr<IMutableTermNode> clone() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

    void accept(MutatingTermVisitorBase &visitor) override;

    bool accept(UnificationVisitor &visitor, const IMutableTermNode &target) const override;

    bool accept(UnificationVisitor &visitor, const MutableFunctionNode &target) const override;

    [[nodiscard]] const std::vector<std::unique_ptr<IMutableTermNode>> &observe_arguments() const;

    std::vector<std::unique_ptr<IMutableTermNode>> &observe_arguments();

    std::ostream &serialise(std::ostream &ostream) const override;

protected:
    const std::string name;

    std::vector<std::unique_ptr<IMutableTermNode>> arguments;
};

}

#endif
