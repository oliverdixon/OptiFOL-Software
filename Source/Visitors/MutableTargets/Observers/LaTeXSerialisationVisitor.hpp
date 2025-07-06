/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/6/25.
//

#ifndef LATEXSERIALISATIONVISITOR_HPP
#define LATEXSERIALISATIONVISITOR_HPP

#include <sstream>

#include "IObservingNodeVisitor.hpp"

namespace optifol
{

enum class QuantifierTypes;
enum class BinaryOperatorTypes;

class LaTeXSerialisationVisitor :
        public IObservingNodeVisitor
{
public:
    void visit(const MutableSentenceRoot &node) override;

    void visit(const MutableIdentity &node) override;

    void visit(const MutableBinaryConnected &node) override;

    void visit(const MutableQuantified &node) override;

    void visit(const MutablePredicate &node) override;

    void visit(const MutableConstant &node) override;

    void visit(const MutableFunction &node) override;

    void visit(const MutableVariable &node) override;

    std::string extract() const;

private:
    [[nodiscard]] static const char *get_quantifier_symbol(QuantifierTypes quantifier) noexcept;

    [[nodiscard]] static const char * get_connected_symbol(BinaryOperatorTypes op) noexcept;

    std::ostringstream latex;
};

} // namespace optifol

#endif // LATEXSERIALISATIONVISITOR_HPP
