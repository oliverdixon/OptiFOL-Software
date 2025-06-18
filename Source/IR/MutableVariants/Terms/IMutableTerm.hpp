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

class MutableFunction;
class MutableVariable;
class UnificationVisitor;
class MutatingTermVisitorBase;

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
     * @brief Recursively
     * @return The transferable container holding the recursively cloned term
     */
    [[nodiscard]] virtual std::unique_ptr<IMutableTerm> clone() const = 0;

    virtual void accept(MutatingTermVisitorBase &visitor) = 0;
};

} // namespace optifol

#endif
