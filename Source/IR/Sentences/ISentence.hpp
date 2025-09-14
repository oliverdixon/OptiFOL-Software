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
};

} // namespace optifol

#endif
