/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Predicate IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEPREDICATE_HPP
#define MUTABLEPREDICATE_HPP

#include <memory>
#include <vector>

#include "../OwningBuildable.hpp"
#include "../Terms/IMutableTerm.hpp"
#include "IMutableSentence.hpp"

namespace optifol
{

class RepositoryBuildingVisitor;

/**
 * @class MutablePredicate
 * @brief A MutablePredicate represents an owning IR node sentence consisting of a display name and zero to many
 *  arguments, all of which are owned as transferable @ref std::unique_ptr objects by the node.
 * @see Predicate for the processed, argument-observing dual.
 */
class MutablePredicate : public IMutableSentence,
                         public OwningBuildable<MutablePredicate>
{
public:
    /**
     * @brief Create an owning predicate with an initial set of owned arguments
     * @param name Display name of the predicate
     * @param is_positive Should the predicate be instantiated with positive polarity?
     * @param arguments A moveable ordered container owning the initial arguments
     */
    explicit MutablePredicate(
            std::string name, bool is_positive, std::vector<std::unique_ptr<IMutableTerm>> &&arguments);

    /**
     * @brief Create an owning predicate with an initial set of owned arguments
     * @param name Display name of the predicate
     * @param is_positive Should the predicate be instantiated with positive polarity?
     * @param arguments A referenced ordered container containing the moveable arguments
     */
    explicit MutablePredicate(
            std::string name, bool is_positive, const std::vector<std::unique_ptr<IMutableTerm>> &arguments = {});

    /**
     * @brief Create an owning unsigned predicate with an initial set of owned arguments
     * @param name Display name of the predicate
     * @param arguments A moveable ordered container owning the initial arguments
     */
    explicit MutablePredicate(std::string name, std::vector<std::unique_ptr<IMutableTerm>> &&arguments);

    /**
     * @brief Create an owning unsigned predicate with an initial set of owned arguments
     * @param name Display name of the predicate
     * @param arguments A referenced ordered container containing the moveable arguments
     */
    explicit MutablePredicate(std::string name, const std::vector<std::unique_ptr<IMutableTerm>> &arguments);

    /**
     * @brief Create an owning unsigned predicate
     * @param name Display name of the predicate
     */
    explicit MutablePredicate(std::string name);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingNodeVisitor &visitor) const override;

    const IProcessedSentence *accept(RepositoryBuildingVisitor &visitor) override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] bool operator==(const IMutableSentence &other) const noexcept override;

    /**
     * @brief Get the display name of the mutable predicate, not including any arguments or metadata
     * @return A view of the predicate symbol name
     */
    [[nodiscard]] std::string_view get_name() const noexcept;

    /**
     * @brief Observe the constant owning ordered argument collection
     * @return The arguments owned by the predicate
     */
    [[nodiscard]] const std::vector<std::unique_ptr<IMutableTerm>> &observe_arguments() const noexcept;

    /**
     * @brief Observe the mutable owning ordered argument collection
     * @return The arguments owned by the predicate
     * @note This non-constant overload is useful for propagation of <code>accept</code> calls on mutating visitors.
     */
    std::vector<std::unique_ptr<IMutableTerm>> &observe_arguments() noexcept;

private:
    const std::string name;
    std::vector<std::unique_ptr<IMutableTerm>> arguments;
    bool is_positive = true;
};

} // namespace optifol

#endif
