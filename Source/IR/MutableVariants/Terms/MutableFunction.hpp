/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Function IR node
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

/**
 * @class MutableFunction
 * @brief A MutableFunction represents an owning IR node term consisting of a display name and zero to many arguments,
 * all of which are owned as transferable @ref std::unique_ptr objects by the node.
 * @see Function for the processed, argument-observing dual.
 */
class MutableFunction : public IMutableTerm, public OwningBuildable<MutableFunction>
{
public:
    /**
     * @brief Create an owning unsigned function with an initial set of owned arguments.
     * @param name Display name of the function
     * @param arguments A moveable ordered container owning the initial arguments
     */
    [[maybe_unused]] explicit MutableFunction(
            std::string name, std::vector<std::unique_ptr<IMutableTerm>> &&arguments = {});

    /**
     * @brief Create an owning unsigned function with an initial set of owned arguments.
     * @param name Display name of the function
     * @param arguments A referenced ordered container containing the moveable arguments
     */
    [[maybe_unused]] explicit MutableFunction(
            std::string name, const std::vector<std::unique_ptr<IMutableTerm>> &arguments = {});

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::unique_ptr<IMutableTerm> clone() const override;

    [[nodiscard]] std::string_view get_disambiguated_name() const override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    void accept(MutatingTermVisitorBase &visitor) override;

    void accept(RepositoryBuildingVisitor &visitor) override;

    /**
     * @brief Observe the constant owning ordered argument collection
     * @return The arguments owned by the function
     */
    [[nodiscard]] const std::vector<std::unique_ptr<IMutableTerm>> &observe_arguments() const noexcept;

    /**
     * @brief Observe the mutable owning ordered argument collection
     * @return The arguments owned by the function
     * @note This non-constant overload is useful for propagation of <code>accept</code> calls on mutating visitors.
     */
    [[nodiscard]] std::vector<std::unique_ptr<IMutableTerm>> &observe_arguments() noexcept;

protected:
    std::string name;
    std::vector<std::unique_ptr<IMutableTerm>> arguments;
};

} // namespace optifol

#endif
