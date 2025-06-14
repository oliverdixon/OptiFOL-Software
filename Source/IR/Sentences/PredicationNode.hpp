/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Predicate IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef PREDICATIONNODE_HPP
#define PREDICATIONNODE_HPP

#include "ISentenceNode.hpp"

#include <memory>
#include <vector>

#include "../../Visitors/Unification/UnificationVisitor.hpp"
#include "../Support/Buildable.hpp"
#include "../../IR/Terms/ITermNode.hpp"

namespace optifol
{

class PredicationNode :
        public ISentenceNode,
        public Buildable<PredicationNode>
{
public:
    explicit PredicationNode(std::string name, bool is_positive,
        std::vector<std::unique_ptr<ITermNode>>&& arguments);

    explicit PredicationNode(std::string name, bool is_positive,
        const std::vector<std::unique_ptr<ITermNode>>& arguments = {});

    explicit PredicationNode(std::string name, std::vector<std::unique_ptr<ITermNode>>&& arguments);

    explicit PredicationNode(std::string name, const std::vector<std::unique_ptr<ITermNode>>& arguments);

    explicit PredicationNode(std::string name);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    bool accept(UnificationVisitor &visitor, const PredicationNode &target) const;

    const std::string name; // TODO move
    std::vector<std::unique_ptr<ITermNode>> arguments; // TODO move

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    bool operator==(const std::unique_ptr<PredicationNode>& other) const
    {
        return other->hash() == hash();
    }

    bool operator==(const std::shared_ptr<PredicationNode>& other) const
    {
        return other->hash() == hash();
    }

private:
    bool is_positive = true;
};

}

#endif
