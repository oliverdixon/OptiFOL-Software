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

#ifndef OPTIFOL_TEXTSERIALISERVISITOR_HPP
#define OPTIFOL_TEXTSERIALISERVISITOR_HPP

#include <sstream>

#include "../IObservingSentenceVisitor.hpp"
#include "../../../IR/Sentences/ISentenceNode.hpp"

namespace optifol
{

enum class BinaryOperatorTypes;
enum class QuantifierTypes;

class TextSerialiserVisitor :
        public IObservingSentenceVisitor
{
public:
    void visit(const QuantifiedSentenceNode& node) override;

    void visit(const ConnectedSentenceNode& node) override;

    void visit(const IdentitySentenceNode& node) override;

    void visit(const PredicationNode& node) override;

    [[nodiscard]] std::string extract();

private:
    std::ostringstream output_stream;

    [[nodiscard]] static const char *get_operator_symbol(BinaryOperatorTypes type);

    [[nodiscard]] static const char *get_operator_symbol(QuantifierTypes type);

    void print_polarity(const ISentenceNode * node);
};

}


#endif
