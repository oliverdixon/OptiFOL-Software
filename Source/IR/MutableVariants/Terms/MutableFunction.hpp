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

#ifndef MUTABLEFUNCTION_HPP
#define MUTABLEFUNCTION_HPP

#include <memory>
#include <vector>

#include "../OwningBuildable.hpp"
#include "IMutableTerm.hpp"

namespace optifol
{

class MutableFunction :
        public IMutableTerm,
        public OwningBuildable<MutableFunction>
{
public:
    [[maybe_unused]] explicit MutableFunction(std::string name, std::vector<std::unique_ptr<IMutableTerm>> &&arguments = {});

    [[maybe_unused]] explicit MutableFunction(std::string name, const std::vector<std::unique_ptr<IMutableTerm>> &arguments);

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::unique_ptr<IMutableTerm> clone() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

    void accept(MutatingTermVisitorBase &visitor) override;

    [[nodiscard]] const std::vector<std::unique_ptr<IMutableTerm>> &observe_arguments() const;

    std::vector<std::unique_ptr<IMutableTerm>> &observe_arguments();

    std::ostream &serialise(std::ostream &ostream) const override;

protected:
    const std::string name;

    std::vector<std::unique_ptr<IMutableTerm>> arguments;
};

}

#endif
