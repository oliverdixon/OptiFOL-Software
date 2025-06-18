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

#ifndef MUTABLECONSTANT_HPP
#define MUTABLECONSTANT_HPP

#include "../OwningBuildable.hpp"
#include "IMutableTerm.hpp"

namespace optifol
{

class MutableConstant : public IMutableTerm, public OwningBuildable<MutableConstant>
{
public:
    [[maybe_unused]] explicit MutableConstant(std::string name);

    [[nodiscard]] std::unique_ptr<IMutableTerm> clone() const override;

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string_view get_disambiguated_name() const override;

    void accept(MutatingTermVisitorBase &visitor) override;

private:
    const std::string name;
};

} // namespace optifol

#endif
