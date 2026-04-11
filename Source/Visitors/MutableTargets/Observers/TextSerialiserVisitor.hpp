/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the text-serialising observing visitor.
 * @author Oliver Dixon
 * @date 2024-11-29
 * @version Development
 */

#ifndef TEXTSERIALISERVISITOR_HPP
#define TEXTSERIALISERVISITOR_HPP

#include <sstream>

#include "../../../IR/MutableVariants/Sentences/IMutableSentence.hpp"
#include "IObservingNodeVisitor.hpp"

namespace optifol
{

enum class BinaryOperatorTypes;
enum class QuantifierTypes;

class TextSerialiserVisitor : public IObservingNodeVisitor
{
public:
    void visit(const MutableQuantified &node) override;

    void visit(const MutableBinaryConnected &node) override;

    void visit(const MutablePredicate &node) override;

    void visit(const MutableSentenceRoot &node) override;

    void visit(const MutableFunction &node) override;

    void visit(const MutableVariable &node) override;

    [[nodiscard]] std::string extract();

private:
    std::ostringstream output_stream;

    void print_polarity(const IMutableSentence *node);
};

} // namespace optifol


#endif
