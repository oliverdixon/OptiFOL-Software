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

#ifndef MUTABLEPREDICATIONNODE_HPP
#define MUTABLEPREDICATIONNODE_HPP

#include "IMutableSentenceNode.hpp"

#include <memory>
#include <vector>

#include "../../Mutable/Terms/IMutableTermNode.hpp"
#include "../OwningBuildable.hpp"

namespace optifol
{

class MutablePredicationNode :
        public IMutableSentenceNode,
        public OwningBuildable<MutablePredicationNode>
{
public:
    explicit MutablePredicationNode(std::string name, bool is_positive,
        std::vector<std::unique_ptr<IMutableTermNode>>&& arguments);

    explicit MutablePredicationNode(std::string name, bool is_positive,
        const std::vector<std::unique_ptr<IMutableTermNode>>& arguments = {});

    explicit MutablePredicationNode(std::string name, std::vector<std::unique_ptr<IMutableTermNode>>&& arguments);

    explicit MutablePredicationNode(std::string name, const std::vector<std::unique_ptr<IMutableTermNode>>& arguments);

    explicit MutablePredicationNode(std::string name);

    [[nodiscard]] std::unique_ptr<IMutableSentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    bool accept(UnificationVisitor &visitor, const MutablePredicationNode &target) const;

    const std::string name; // TODO move
    std::vector<std::unique_ptr<IMutableTermNode>> arguments; // TODO move

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    bool operator==(const std::unique_ptr<MutablePredicationNode>& other) const
    {
        return other->hash() == hash();
    }

    bool operator==(const std::shared_ptr<MutablePredicationNode>& other) const
    {
        return other->hash() == hash();
    }

private:
    bool is_positive = true;
};

}

#endif
