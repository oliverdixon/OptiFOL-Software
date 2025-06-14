/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the JSON-serialising observing visitor.
 * @author Oliver Dixon
 * @date 2024-11-29
 * @version Development
 */

#ifndef JSONSERIALISERVISITOR_HPP
#define JSONSERIALISERVISITOR_HPP

#include <nlohmann/json.hpp>

#include "../../../IR/Mutable/Sentences/IMutableSentenceNode.hpp"
#include "../IObservingSentenceVisitor.hpp"

namespace optifol
{

enum class BinaryOperatorTypes;
enum class QuantifierTypes;

class JSONSerialiserVisitor:
        public IObservingSentenceVisitor
{
public:
    void visit(const MutableQuantifiedSentenceNode& node) override;

    void visit(const MutableConnectedSentenceNode& node) override;

    void visit(const MutableIdentitySentenceNode& node) override;

    void visit(const MutablePredicationNode& node) override;

    void visit(const MutableSentenceRoot& node) override;

    [[nodiscard]] nlohmann::json extract();

private:
    nlohmann::json output;

    [[nodiscard]] static const char *get_operator_symbol(BinaryOperatorTypes type);

    [[nodiscard]] static const char *get_operator_symbol(QuantifierTypes type);

    void print_polarity(const IMutableSentenceNode * node);
};

}

#endif
