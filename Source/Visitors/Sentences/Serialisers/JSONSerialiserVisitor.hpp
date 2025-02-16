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

#ifndef OPTIFOL_JSONSERIALISERVISITOR_HPP
#define OPTIFOL_JSONSERIALISERVISITOR_HPP

#include <nlohmann/json.hpp>

#include "../IObservingSentenceVisitor.hpp"

namespace optifol
{

enum class BinaryOperatorTypes;
enum class QuantifierTypes;

class JSONSerialiserVisitor:
        public IObservingSentenceVisitor
{
public:
    void visit(const QuantifiedSentenceNode& node) override;

    void visit(const ConnectedSentenceNode& node) override;

    void visit(const NegatedSentenceNode& node) override;

    void visit(const NodeProxy& node) override;

    void visit(const IdentitySentenceNode& node) override;

    void visit(const PredicationNode& node) override;

    [[nodiscard]] nlohmann::json extract();

private:
    nlohmann::json output;

    [[nodiscard]] static const char *get_operator_symbol(BinaryOperatorTypes type);

    [[nodiscard]] static const char *get_operator_symbol(QuantifierTypes type);
};

}

#endif
