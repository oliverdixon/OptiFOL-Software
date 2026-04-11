/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Skolem Function IR node
 * @author Oliver Dixon
 * @date 2025-01-30
 * @version Development
 */

#ifndef OPTIFOL_SKOLEMFUNCTION_HPP
#define OPTIFOL_SKOLEMFUNCTION_HPP

#include "Function.hpp"

namespace optifol
{

/**
 * @class SkolemFunction
 * @brief A Skolem Function is a Function symbol used to preserve equivalence with existentially quantified expressions
 *  in CNF. It is typically introduced by the normalisation pipeline.
 */
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
