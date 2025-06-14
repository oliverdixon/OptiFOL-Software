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

#ifndef ISENTENCENODE_HPP
#define ISENTENCENODE_HPP

#include <memory>

#include "../../../IHashable.hpp"

namespace optifol
{

class MutatingSentenceVisitorBase;
class IObservingSentenceVisitor;

class IMutableSentenceNode :
        public IHashable
{
public:
    ~IMutableSentenceNode() override = default;

    virtual void flip_polarity() = 0;

    [[nodiscard]] virtual bool is_negative_polarity() const = 0;

    [[nodiscard]] virtual std::unique_ptr<IMutableSentenceNode> clone() const = 0;

    virtual void accept(MutatingSentenceVisitorBase &visitor) = 0;

    virtual void accept(IObservingSentenceVisitor &visitor) const = 0;

    [[nodiscard]] std::size_t hash() const noexcept override = 0;

    /**
     * @brief Serialise a basic representation of the IMutableSentenceNode object on the given output stream
     * @param ostream The destination output stream
     * @return The output stream populated with the serialised sentence
     * @note This function is provided for satisfaction of \ref GoogleTestable.
     */
    virtual std::ostream &serialise(std::ostream &ostream) const = 0;

    /**
     * @brief Serialise a basic representation of the ISentenceNode object on the given output stream
     * @param ostream The destination output stream
     * @param object The sentence to serialise
    * @note This function is provided for satisfaction of \ref GoogleTestable.
     */
    friend std::ostream& operator<<(std::ostream& ostream, const IMutableSentenceNode& object)
    {
        return object.serialise(ostream);
    }

    /**
     * @brief Test equality between two sentences
     * @param other The sentence against which to compare for equality
     * @return Is the current sentence equal to the given sentence?
     * @note The metric of equality is hash-based
     * @note This function is provided for satisfaction of \ref GoogleTestable.
     */
    [[nodiscard]] bool operator==(const IMutableSentenceNode& other) const
    {
        return hash() == other.hash();
    }
};

}

#endif
