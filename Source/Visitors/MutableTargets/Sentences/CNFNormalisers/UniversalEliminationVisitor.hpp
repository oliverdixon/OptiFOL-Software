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

#include "../../../../IR/MutableVariants/Sentences/IMutableSentence.hpp"
#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

class IMutableSentence;

class UniversalEliminationVisitor : public MutatingSentenceVisitorBase
{
public:
    [[nodiscard]] std::string_view get_visitor_name() const override;

    void visit(MutableQuantified &node) override;

    void visit(MutableBinaryConnected &node) override;

    void visit(MutableSentenceRoot &node) override;

private:
    static const char *visitor_name;

    std::unique_ptr<IMutableSentence> extracted_sentence;
};

} // namespace optifol

#endif
