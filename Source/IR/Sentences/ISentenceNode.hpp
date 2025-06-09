/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic IR Sentence interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef ISENTENCENODE_HPP
#define ISENTENCENODE_HPP

#include <memory>

#include "../../IHashable.hpp"

namespace optifol
{

class MutatingSentenceVisitorBase;
class IObservingSentenceVisitor;

class ISentenceNode :
        public IHashable
{
public:
    ~ISentenceNode() override = default;

    virtual void flip_polarity() = 0;

    [[nodiscard]] virtual bool is_negative_polarity() const = 0;

    [[nodiscard]] virtual std::unique_ptr<ISentenceNode> clone() const = 0;

    virtual void accept(MutatingSentenceVisitorBase &visitor) = 0;

    virtual void accept(IObservingSentenceVisitor &visitor) const = 0;

    [[nodiscard]] std::size_t hash() const noexcept override = 0;

    /**
     * @brief Serialise a basic representation of the ISentenceNode object on the given output stream
     * @param ostream The destination output stream
     * @return The output stream populated with the serialised sentence
     * @warning Serialisation should normally be done via the TextSerialiserVisitor (or JSONSerialiserVisitor for JSON),
     *  but the serialisation functions on ISentenceNode are provided for compatibility with third-party frameworks that
     *  expected standard member functions and operator overloads. For example, Google Test uses operator<< to produce
     *  readable error reports when an assertion fails.
     */
    virtual std::ostream &serialise(std::ostream &ostream) const = 0;

    /**
     * @brief Serialise a basic representation of the ISentenceNode object on the given output stream
     * @param ostream The destination output stream
     * @param object The sentence to serialise
     * @return The output stream populated with the serialised sentence
     */
    friend std::ostream& operator<<(std::ostream& ostream, const ISentenceNode& object)
    {
        return object.serialise(ostream);
    }

    /**
     * @brief Test equality between two sentences
     * @param other The sentence against which to compare for equality
     * @return Is the current sentence equal to the given sentence?
     * @note The metric of equality is hash-based
     * @warning Equality should be determined through a standard visitor. This member function operator overload is
     *  provided for compatibility with third-party libraries. Google Test, for instance, uses operator== to support its
     *  EXPECT_EQ assertion macro.
     */
    [[nodiscard]] bool operator==(const ISentenceNode& other) const
    {
        return hash() == other.hash();
    }
};

}

#endif
