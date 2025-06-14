/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <optional>

#include "IProcessedTerm.hpp"

namespace optifol
{

class Variable :
        public IProcessedTerm
{
public:
    explicit Variable(std::string name);

    explicit Variable(std::string name, const std::string& disambiguated_name);

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

private:
    const std::string name;

    std::optional<std::string> disambiguated_name;
};

}

#endif
