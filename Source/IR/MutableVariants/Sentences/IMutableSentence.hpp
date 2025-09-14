/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic mutable IR Sentence interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef IMUTABLESENTENCE_HPP
#define IMUTABLESENTENCE_HPP

#include <memory>

#include "../../Sentences/ISentence.hpp"

namespace optifol
{

class MutatingSentenceVisitorBase;
class IObservingNodeVisitor;
class RepositoryBuildingVisitor;
class IProcessedSentence;

/**
 * @class IMutableSentence
 * @brief A mutable sentence is a sentence IR node with properties useful during parsing and normalisation, prior to
 *  formal analysis.
 * @details A mutable sentence, in general, has the following properties:
 *  <ul>
 *      <li>
 *          Any child nodes (e.g. arguments to a MutablePredicate, bound sentences to MutableQuantified, etc.) are
 *          strictly owned by explicitly transferable containers, typically a @ref std::unique_ptr. Immutable observing
 *          pointers can be extracted without requiring a non-constant object, but for mutation ownership needs to be
 *          explicitly stolen from the sentence node and later returned through <code>steal</code> and <code>put</code>
 *          member functions.
 *      </li>
 *      <li>
 *          POD metadata (e.g. display names and operator types for MutableBinaryConnected) are changeable on
 *          non-constant objects, and need not persist the same values for the lifetime of the object.
 *      </li>
 *  </ul>
 */
class IMutableSentence : public ISentence
{
public:
    /**
     * @brief Perform a deep-copy of the sentence and produce a copy wrapped in a transferable @ref std::unique_ptr.
     * @return The container containing the copied sentence.
     */
    [[nodiscard]] virtual std::unique_ptr<IMutableSentence> clone() const = 0;

    /**
     * @brief Flip the polarity of the instantiated sentence
     * @see ISentence::is_negative_polarity for dual getter
     */
    virtual void flip_polarity() noexcept = 0;

    /**
     * @brief Accept a visitation from a mutating sentence visitor on the object
     * @param visitor The mutating visitor to invoke
     */
    virtual void accept(MutatingSentenceVisitorBase &visitor) = 0;

    /**
     * @brief Accept a visitation from an observing (non-mutating) sentence visitor on the object
     * @param visitor The observing visitor to invoke
     */
    virtual void accept(IObservingNodeVisitor &visitor) const = 0;

    /**
     * @brief Accept a visitation from a mutating RepositoryBuildingVisitor on the object
     * @param visitor The repository-building visitor to invoke
     * @return An observing reference to the ISentence node added to a central SymbolRepository object
     */
    virtual const IProcessedSentence *accept(RepositoryBuildingVisitor &visitor) = 0;

    /**
     * @brief Test equality between two IMutableSentence instances.
     * @param other The other IMutableSentence instance with which to test for equality.
     * @return Are the two sentences of the same concrete type and equal?
     */
    [[nodiscard]] virtual bool operator==(const IMutableSentence &other) const noexcept = 0;
};

} // namespace optifol

#endif
