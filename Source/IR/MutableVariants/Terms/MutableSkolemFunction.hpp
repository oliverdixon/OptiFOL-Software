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

#ifndef MUTABLESKOLEMFUNCTION_HPP
#define MUTABLESKOLEMFUNCTION_HPP

#include <vector>

#include "MutableFunction.hpp"

namespace optifol
{

class MutableVariable;

class MutableSkolemFunction : public MutableFunction
{
public:
    [[maybe_unused]] explicit MutableSkolemFunction(
            std::string name, std::vector<std::unique_ptr<IMutableTerm>> &&quantified_variables = {});

    [[nodiscard]] std::unique_ptr<IMutableTerm> clone() const override;

    const IProcessedTerm *accept(RepositoryBuildingVisitor &visitor) override;
};

} // namespace optifol

#endif
