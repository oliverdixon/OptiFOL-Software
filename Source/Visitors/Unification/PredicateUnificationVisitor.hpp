/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef PREDICATEUNIFICATIONVISITOR_H
#define PREDICATEUNIFICATIONVISITOR_H
#include <optional>

#include "../../IR/Support/Substitution.hpp"

namespace optifol
{

class FunctionNode;
class ITermNode;
class VariableNode;
class PredicationNode;

class PredicateUnificationVisitor
{
public:
    void visit(PredicationNode& predicate_lhs, PredicationNode& predicate_rhs);

    void visit(VariableNode& variable_lhs, ITermNode& generic_term_rhs);

    void visit(VariableNode& variable_lhs, VariableNode& variable_rhs);

    void visit(FunctionNode& function_lhs, FunctionNode& function_rhs);

    void visit(ITermNode& generic_term_lhs, VariableNode& variable_rhs);

private:
    std::optional<Substitution> replacements;
};

}

#endif
