/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the IR Processed Term interface
 * @author Oliver Dixon
 * @date 2025-06-14
 * @version Development
 */

#ifndef IPROCESSEDTERM_HPP
#define IPROCESSEDTERM_HPP

#include "ITerm.hpp"

namespace optifol
{

class FeatureBuildingVisitor;

class IObservingBinaryVisitor;
class UnificationApplicationVisitor;
class RepositoryBuildingVisitor;

class Function;
class Variable;

/**
 * @class IProcessedTerm
 * @brief An IProcessedTerm is an ITerm that has undergone the lexing, parsing, and normalisation pipeline and
 * is now held, in its disambiguated form, in a centralised SymbolRepository.
 * @details Such terms are always immutable and have lifetime guarantees in accordance with their responsible
 *  SymbolRepository.
 */
class IProcessedTerm : public ITerm
{
public:
    /**
     * @brief Accept a visitation from an IObservingBinaryVisitor, despatching dynamically on the given term
     * as the second operand.
     * @param binary_visitor The binary visitor from whom to accept a visit.
     * @param term The generic term for the second operand.
     * @return Visitor return code.
     */
    [[nodiscard]] virtual bool accept(
            IObservingBinaryVisitor &binary_visitor, const IProcessedTerm &term) const = 0;

    /**
     * @brief Accept a visitation from an IObservingBinaryVisitor, using the given Function as the second
     * operand.
     * @param binary_visitor The binary visitor from whom to accept a visit.
     * @param function The Function term for the second operand.
     * @return Visitor return code.
     */
    [[nodiscard]] virtual bool accept(
            IObservingBinaryVisitor &binary_visitor, const Function &function) const = 0;

    /**
     * @brief Accept a visitation from an IObservingBinaryVisitor, using the given Variable as the second
     * operand.
     * @param binary_visitor The binary visitor from whom to accept a visit.
     * @param variable The Variable term for the second operand.
     * @return Visitor return code.
     */
    [[nodiscard]] virtual bool accept(
            IObservingBinaryVisitor &binary_visitor, const Variable &variable) const = 0;

    virtual void accept(FeatureBuildingVisitor &feature_building_visitor) const noexcept = 0;

    /**
     * @brief Accept a visitation from an IObservingBinaryVisitor, using the given Function as the first
     * operand.
     * @param binary_visitor The binary visitor from whom to accept a visit.
     * @param function The Function term for the first operand.
     * @return Visitor return code.
     */
    [[nodiscard]] virtual bool accept_reverse(
            IObservingBinaryVisitor &binary_visitor, const Function &function) const = 0;

    /**
     * @brief Accept a visitation from an IObservingBinaryVisitor, using the given Variable as the first
     * operand.
     * @param binary_visitor The binary visitor from whom to accept a visit.
     * @param variable The Variable term for the first operand.
     * @return Visitor return code.
     */
    [[nodiscard]] virtual bool accept_reverse(
            IObservingBinaryVisitor &binary_visitor, const Variable &variable) const = 0;

    /**
     * @brief Determines whether the given Variable appears in the expansion of the term.
     * @param search_term The Variable for which to search in the term.
     * @return Does the given Variable appear in the IProcessedTerm or any trivial expansion thereof?
     */
    [[nodiscard]] virtual bool is_self_nested(const IProcessedTerm &search_term) const noexcept = 0;

    /**
     * @brief Accept a visit from the UnificationApplicationVisitor to construct new IProcessedTerm objects by
     *  substituting Variable instances into the term.
     * @param unification_application_visitor The non-mutating visitor to accept.
     * @see UnificationApplicationVisitor for the visitor context.
     */
    [[nodiscard]] virtual const IProcessedTerm *accept(
            const UnificationApplicationVisitor &unification_application_visitor) const = 0;

    /**
     * @brief Test equality between two IProcessedTerm instances.
     * @param other The other IProcessedTerm instance with which to test for equality.
     * @return Are the two terms of the same concrete type and equal?
     */
    [[nodiscard]] virtual bool operator==(const IProcessedTerm &other) const noexcept = 0;

    /**
     * @brief Test ordering between two IProcessedTerm instances.
     * @param other The other IProcessedTerm instance to compare against.
     * @return Am I strictly less than the other term?
     */
    [[nodiscard]] virtual bool operator<(const IProcessedTerm &other) const noexcept = 0;
};

} // namespace optifol

#endif // IPROCESSEDTERM_HPP
