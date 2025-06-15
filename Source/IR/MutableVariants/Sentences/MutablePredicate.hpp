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

#include "IMutableSentence.hpp"
#include "../Terms/IMutableTerm.hpp"
#include "../OwningBuildable.hpp"

namespace optifol
{

/**
 * @class MutablePredicate
 * @brief A MutablePredicate represents an owning IR node sentence consisting of a display name and zero to many
 *  arguments, all of which are owned as transferable @ref std::unique_ptr objects by the node.
 * @see Predicate for the processed, argument-observing dual.
 */
class MutablePredicate :
        public IMutableSentence,
        public OwningBuildable<MutablePredicate>
{
public:
    /**
     * @brief Create an owning predicate with an initial set of owned arguments
     * @param name Display name of the predicate
     * @param is_positive Should the predicate be instantiated with positive polarity?
     * @param arguments A moveable ordered container owning the initial arguments
     */
    explicit MutablePredicate(std::string name, bool is_positive,
        std::vector<std::unique_ptr<IMutableTerm>>&& arguments);

    /**
     * @brief Create an owning predicate with an initial set of owned arguments
     * @param name Display name of the predicate
     * @param is_positive Should the predicate be instantiated with positive polarity?
     * @param arguments A referenced ordered container containing the moveable arguments
     */
    explicit MutablePredicate(std::string name, bool is_positive,
        const std::vector<std::unique_ptr<IMutableTerm>>& arguments = {});

    /**
     * @brief Create an owning unsigned predicate with an initial set of owned arguments
     * @param name Display name of the predicate
     * @param arguments A moveable ordered container owning the initial arguments
     */
    explicit MutablePredicate(std::string name, std::vector<std::unique_ptr<IMutableTerm>>&& arguments);

    /**
     * @brief Create an owning unsigned predicate with an initial set of owned arguments
     * @param name Display name of the predicate
     * @param arguments A referenced ordered container containing the moveable arguments
     */
    explicit MutablePredicate(std::string name, const std::vector<std::unique_ptr<IMutableTerm>>& arguments);

    /**
     * @brief Create an owning unsigned predicate
     * @param name Display name of the predicate
     */
    explicit MutablePredicate(std::string name);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    const std::string name; // TODO move
    std::vector<std::unique_ptr<IMutableTerm>> arguments; // TODO move

private:
    bool is_positive = true;
};

}

#endif
