/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic IR Sentence interface
 * @author Oliver Dixon
 * @date 2025-06-14
 * @version Development
 */

#ifndef ISENTENCE_HPP
#define ISENTENCE_HPP

#include "../../IHashable.hpp"

namespace optifol
{

/**
 * @class ISentence
 * @brief A sentence represents a first-order logic sentence node within an IR tree. In most instantiations, it may
 *  consist of a deeply nested structure and always supports a polarity, i.e. positive or negative.
 */
class ISentence : public IHashable
{
public:
    [[nodiscard]] virtual bool is_negative_polarity() const noexcept = 0;

    /**
     * @brief Serialise a basic representation of the ISentence object on the given output stream
     * @param ostream The destination output stream
     * @return The output stream populated with the serialised sentence
     * @note This function is provided for satisfaction of \ref GoogleTestable.
     */
    virtual std::ostream &serialise(std::ostream &ostream) const = 0;

    /**
     * @brief Serialise a basic representation of the ISentence object on the given output stream
     * @param ostream The destination output stream
     * @param object The sentence to serialise
     * @note This function is provided for satisfaction of @ref GoogleTestable.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const ISentence &object)
    {
        return object.serialise(ostream);
    }

    /**
     * @brief Test equality between two sentences
     * @param other The sentence against which to compare for equality
     * @return Is the current sentence equal to the given sentence?
     * @note The metric of equality is hash-based
     * @note This function is provided for satisfaction of @ref GoogleTestable.
     */
    [[nodiscard]] bool operator==(const ISentence &other) const
    {
        return hash() == other.hash();
    }

    /**
     * @brief Compare with another sentence wrapped in a @ref std::unique_ptr
     * @param other The wrapper containing the sentence against which equality should be determined
     * @return Is the wrapped sentence hash-equal to us?
     * @see GoogleTestable for desired concept
     */
    bool operator==(const std::unique_ptr<ISentence> &other) const noexcept
    {
        return hash() == other->hash();
    }

    /**
     * @brief Compare with another sentence wrapped in a @ref std::shared_ptr
     * @param other The wrapper containing the sentence against which equality should be determined
     * @return Is the wrapped sentence hash-equal to us?
     * @see TransparentlyHashable for rationale
     */
    bool operator==(const std::shared_ptr<ISentence> &other) const noexcept
    {
        return hash() == other->hash();
    }
};

} // namespace optifol

#endif
