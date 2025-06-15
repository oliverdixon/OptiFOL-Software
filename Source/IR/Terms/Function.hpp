/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Function IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>

#include "IProcessedTerm.hpp"

namespace optifol
{

/**
 * @class Function
 * @brief A Function represents a non-owning IR node term consisting of a display name and zero to many arguments, all
 *  of which are referenced from the centralised SymbolRepository.
 * @see MutableFunction for the unprocessed, argument-owning dual.
 */
class Function :
        public IProcessedTerm
{
public:
    /**
     * @brief Create a non-owning unsigned function with an initial set of referenced immutable arguments.
     * @param name Display name of the function
     * @param arguments Set of non-owning pointers to immutable arguments
     */
    [[maybe_unused]] explicit Function(std::string name, std::initializer_list<const IProcessedTerm *> arguments = {});

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string get_disambiguated_name() const override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    /**
     * @brief Observe the non-owning ordered argument collection
     * @return The arguments referenced by the function
     */
    [[nodiscard]] const std::vector<const IProcessedTerm *> &observe_arguments() const noexcept;

private:
    const std::string name;

    std::vector<const IProcessedTerm *> arguments;
};

}

#endif
