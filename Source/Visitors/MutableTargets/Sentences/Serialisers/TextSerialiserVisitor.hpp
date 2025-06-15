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

#include "../../../../IR/MutableVariants/Sentences/IMutableSentence.hpp"
#include "../IObservingSentenceVisitor.hpp"

namespace optifol
{

enum class BinaryOperatorTypes;
enum class QuantifierTypes;

class TextSerialiserVisitor :
        public IObservingSentenceVisitor
{
public:
    void visit(const MutableQuantified& node) override;

    void visit(const MutableBinaryConnected& node) override;

    void visit(const MutableIdentity& node) override;

    void visit(const MutablePredicate& node) override;

    void visit(const MutableSentenceRoot& node) override;

    [[nodiscard]] std::string extract();

    [[nodiscard]] static const char *get_operator_symbol(QuantifierTypes type);

private:
    std::ostringstream output_stream;

    void print_polarity(const IMutableSentence * node);
};

}


#endif
