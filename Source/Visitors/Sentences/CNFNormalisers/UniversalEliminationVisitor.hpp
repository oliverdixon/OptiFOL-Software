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

#ifndef OPTIFOL_UNIVERSALELIMINATIONVISITOR_HPP
#define OPTIFOL_UNIVERSALELIMINATIONVISITOR_HPP

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

    void visit(NodeProxy& proxy) override;

    void reset() override;

private:
    std::shared_ptr<ISentenceNode> extracted_sentence;
};

}

#endif
