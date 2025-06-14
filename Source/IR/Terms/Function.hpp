/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>

#include "IProcessedTerm.hpp"

namespace optifol
{

class Function :
        public IProcessedTerm
{
public:
    [[maybe_unused]] explicit Function(std::string name, std::initializer_list<const IProcessedTerm *> arguments = {});

    const std::vector<const IProcessedTerm *> &observe_arguments() const noexcept;

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    const std::string name;

    std::vector<const IProcessedTerm *> arguments;
};

}

#endif
