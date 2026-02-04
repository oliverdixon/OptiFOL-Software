/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/30/26.
//

#ifndef OPTIFOL_SKOLEMFUNCTION_HPP
#define OPTIFOL_SKOLEMFUNCTION_HPP

#include "Function.hpp"

namespace optifol
{

class SkolemFunction : public Function
{
public:
    [[maybe_unused]] explicit SkolemFunction(std::string name,
        std::initializer_list<const IProcessedTerm *> arguments = {});

    explicit SkolemFunction(std::string name, std::vector<const IProcessedTerm *>&& arguments);

    void accept(FeatureBuildingVisitor& feature_building_visitor) const noexcept override;
};

} // namespace optifol

#endif // OPTIFOL_SKOLEMFUNCTION_HPP
