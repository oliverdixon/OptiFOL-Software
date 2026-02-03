/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Interface specification for the FOL binary visitor
 * @author Oliver Dixon
 * @date 2026-02-02
 * @version Development
 */

#ifndef OPTIFOL_IOBSERVINGBINARYVISITOR_HPP
#define OPTIFOL_IOBSERVINGBINARYVISITOR_HPP

namespace optifol
{

class Literal;
class Constant;
class Variable;
class Function;

class IObservingBinaryVisitor
{
public:
    virtual ~IObservingBinaryVisitor() = default;

    // Literal

    [[nodiscard]] virtual bool visit(const Literal &predicate_lhs, const Literal &predicate_rhs) = 0;
    
    // Variable on LHS
    
    [[nodiscard]] virtual bool visit(const Variable &variable_lhs, const Constant &constant_rhs) = 0;

    [[nodiscard]] virtual bool visit(const Variable &variable_lhs, const Function &function_rhs) = 0;
    
    [[nodiscard]] virtual bool visit(const Variable &variable_lhs, const Variable &variable_rhs) = 0;
    
    // Constant on LHS
    
    [[nodiscard]] virtual bool visit(const Constant &constant_lhs, const Constant &constant_rhs) = 0;
    
    [[nodiscard]] virtual bool visit(const Constant &constant_lhs, const Function &function_rhs) = 0;
    
    [[nodiscard]] virtual bool visit(const Constant &constant_lhs, const Variable &variable_rhs) = 0;
    
    // Function on LHS
    
    [[nodiscard]] virtual bool visit(const Function &function_lhs, const Constant &constant_rhs) = 0;
    
    [[nodiscard]] virtual bool visit(const Function &function_lhs, const Function &function_rhs) = 0;

    [[nodiscard]] virtual bool visit(const Function &function_lhs, const Variable &variable_rhs) = 0;
};

} // namespace optifol

#endif // OPTIFOL_IOBSERVINGBINARYVISITOR_HPP
