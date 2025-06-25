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

class RepositoryBuildingVisitor;

/**
 * @class IProcessedTerm
 * @brief An IProcessedTerm is an ITerm that has undergone the lexing, parsing, and normalisation pipeline and is now
 *  held, in its disambiguated form, in a centralised SymbolRepository.
 * @details Such terms are always immutable and have lifetime guarantees in accordance with their responsible
 *  SymbolRepository. They are also capable of accepting visits from visitors with immutable (a.k.a. "regular") targets,
 *  such as the UnificationVisitor.
 */
class IProcessedTerm : public ITerm
{
public:
    /**
     * @brief Accept a UnificationVisitor request for unification with a generic IProcessedTerm
     * @param visitor The UnificationVisitor instance conducting the unification
     * @param target The target node with which unification should be attempted
     * @return Did the UnificationVisitor indicate successful unification?
     */
    virtual bool accept(UnificationVisitor &visitor, const IProcessedTerm &target) const;

    /**
     * @brief Accept a UnificationVisitor request for unification with a Function term
     * @param visitor The UnificationVisitor instance conducting the unification
     * @param target The target node with which unification should be attempted
     * @return Did the UnificationVisitor indicate successful unification?
     */
    virtual bool accept(UnificationVisitor &visitor, const Function &target) const;

    /**
     * @brief Test hash-based equality with another IProcessedTerm, wrapped in a @ref std::unique_ptr
     * @param other The owning container of the IProcessedTerm with which equality should be tested
     * @return Are the IProcessedTerm objects equal?
     */
    bool operator==(const std::unique_ptr<IProcessedTerm> &other) const noexcept
    {
        return other->hash() == hash();
    }

    bool operator==(const IProcessedTerm &other) const noexcept
    {
        return other.hash() == hash();
    }
};

} // namespace optifol

#endif // IPROCESSEDTERM_HPP
