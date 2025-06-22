/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic mutable IR Term interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef IMUTABLETERM_HPP
#define IMUTABLETERM_HPP

#include <memory>

#include "../../Terms/ITerm.hpp"

namespace optifol
{

class MutatingTermVisitorBase;
class RepositoryBuildingVisitor;

/**
 * @class IMutableTerm
 * @brief An IMutableTerm is an ITerm that has not undergone the full lexing, parsing, and normalisation pipeline. Such
 *  terms are still being processed and likely to be mutated in-situ.
 * @details IMutableTerms are typically held within an outermost @ref std::unique_ptr to enforce clear semantics of
 *  ownership, and the explicit transfer thereof, throughout the mutation pipelines. Once an IMutableTerm has been
 *  deemed as processed, with no further mutations necessary, it should be converted to an IProcessedTerm that has more
 *  restrictions but does not require equally stringent ownership.
 */
class IMutableTerm : public ITerm
{
public:
    /**
     * @brief Recursively clone an owning term, making replicas of all children held by the cloned parent, and return
     *  the root-most node detained by a @ref std::unique_ptr.
     * @return The transferable container holding the recursively cloned term
     */
    [[nodiscard]] virtual std::unique_ptr<IMutableTerm> clone() const = 0;

    /**
     * @brief Accept a visitation from a MutatingTermVisitorBase-type visitor
     * @param visitor The instantiation of the mutating term visitor
     */
    virtual void accept(MutatingTermVisitorBase &visitor) = 0;

    /**
     * @brief Accept a visitation from a RepositoryBuildingVisitor-type visitor
     * @param visitor The instantiation of the mutating repository-building visitor
     */
    virtual void accept(RepositoryBuildingVisitor& visitor) = 0;
};

} // namespace optifol

#endif
