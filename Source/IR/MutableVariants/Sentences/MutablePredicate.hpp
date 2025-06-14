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

class MutablePredicate :
        public IMutableSentence,
        public OwningBuildable<MutablePredicate>
{
public:
    explicit MutablePredicate(std::string name, bool is_positive,
        std::vector<std::unique_ptr<IMutableTerm>>&& arguments);

    explicit MutablePredicate(std::string name, bool is_positive,
        const std::vector<std::unique_ptr<IMutableTerm>>& arguments = {});

    explicit MutablePredicate(std::string name, std::vector<std::unique_ptr<IMutableTerm>>&& arguments);

    explicit MutablePredicate(std::string name, const std::vector<std::unique_ptr<IMutableTerm>>& arguments);

    explicit MutablePredicate(std::string name);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    const std::string name; // TODO move
    std::vector<std::unique_ptr<IMutableTerm>> arguments; // TODO move

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    bool is_positive = true;
};

}

#endif
