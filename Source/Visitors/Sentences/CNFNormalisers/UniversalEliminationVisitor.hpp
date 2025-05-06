/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Universal-Elimination Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-25
 * @version Development
 */

#ifndef UNIVERSALELIMINATIONVISITOR_HPP
#define UNIVERSALELIMINATIONVISITOR_HPP

#include <memory>

#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

class ISentenceNode;

class UniversalEliminationVisitor :
        public MutatingSentenceVisitorBase
{
public:
    void visit(QuantifiedSentenceNode& node) override;

    void visit(ConnectedSentenceNode &node) override;

    void visit(SentenceRoot& node) override;

private:
    std::unique_ptr<ISentenceNode> extracted_sentence;
};

}

#endif
