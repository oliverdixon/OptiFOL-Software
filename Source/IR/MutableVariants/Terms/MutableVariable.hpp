/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Variable Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEVARIABLE_HPP
#define MUTABLEVARIABLE_HPP

#include <optional>

#include "../OwningBuildable.hpp"
#include "IMutableTerm.hpp"

namespace optifol
{

class MutableVariable :
        public IMutableTerm,
        public OwningBuildable<MutableVariable>
{
public:
    explicit MutableVariable(std::string name);

    explicit MutableVariable(std::string name, const std::string& disambiguated_name);

    [[nodiscard]] std::unique_ptr<IMutableTerm> clone() const override;

    void accept(MutatingTermVisitorBase& visitor) override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

private:
    const std::string name;

    std::optional<std::string> disambiguated_name;
};

}

#endif
