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

#include "../Support/UnifyCandidateBase.hpp"

namespace optifol
{

class ITermNode;

class PredicationNode :
        public ISentenceNode,
        public UnifyCandidateBase
{
public:
    [[maybe_unused]] PredicationNode(std::string name, std::vector<std::unique_ptr<ITermNode>> &&arguments,
        bool is_positive = true);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    bool unify_work(const PredicationNode &predicate) override;

    const std::string name; // TODO move
    std::vector<std::unique_ptr<ITermNode>> arguments; // TODO move

    [[nodiscard]] std::size_t hash() const noexcept override;

private:
    bool is_positive;
};

}

#endif
