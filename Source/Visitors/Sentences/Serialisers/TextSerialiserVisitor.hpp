/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file TextSerialiserVisitor.hpp
 * @brief Class specification for the text-serialising observing visitor.
 * @author Oliver Dixon
 * @date 2024-11-29
 * @version Development
 */

#ifndef OPTIFOL_TEXTSERIALISERVISITOR_HPP
#define OPTIFOL_TEXTSERIALISERVISITOR_HPP

#include <sstream>

#include "../IObservingSentenceVisitor.hpp"

namespace optifol
{

enum class BinaryOperatorTypes;
enum class QuantifierTypes;

class TextSerialiserVisitor :
        public IObservingSentenceVisitor
{
public:
    using OutT = std::string;

    void visit(const QuantifiedSentenceNode& node) override;

    void visit(const ConnectedSentenceNode& node) override;

    void visit(const NegatedSentenceNode& node) override;

    void visit(const NodeProxy& node) override;

    void visit(const IdentitySentenceNode& node) override;

    void visit(const PredicationNode& node) override;

    std::any extract() const override;

    void reset() override;

private:
    std::ostringstream output_stream;

    [[nodiscard]] static const char *get_operator_symbol(BinaryOperatorTypes type);

    [[nodiscard]] static const char *get_operator_symbol(QuantifierTypes type);
};

}


#endif //OPTIFOL_TEXTSERIALISERVISITOR_HPP
