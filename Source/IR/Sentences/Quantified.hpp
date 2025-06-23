/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Quantified Sentence IR node
 * @author Oliver Dixon
 * @date 2025-06-17
 * @version Development
 */

#ifndef QUANTIFIED_HPP
#define QUANTIFIED_HPP

#include "ISentence.hpp"

namespace optifol
{

class Variable;
class ITerm;

/**
 * @class QuantifierTypes
 * @brief The modes by which a sentence can be bound by a variable
 */
enum class QuantifierTypes
{
    Universal,
    Existential
};

/**
 * @class Quantified
 * @brief A Quantified IR node references a bound variable/term and a bound sentence ("operands"), and detains the
 *  first-order logic operator by which the sentence is bound by the variable. Metadata and operands are immutable;
 *  operands are held centrally in a SymbolRepository.
 */
class Quantified : public ISentence
{
public:
    /**
     * @brief Creates an immutable Quantified Sentence IR node with the given attributes
     * @param quantifier_type The type of QuantifierTypes with which the sentence should be bound
     * @param bound_term The term to bind
     * @param sentence The sentence to bind
     * @param is_positive Is the quantified instantiation positive?
     */
    Quantified(QuantifierTypes quantifier_type, const IProcessedTerm *bound_term, const ISentence *sentence,
            bool is_positive = true);

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    /**
     * @brief Retrieve the quantification operator that binds the variable and the sentence
     * @return The quantifier type
     */
    [[nodiscard]] QuantifierTypes get_quantifier_type() const noexcept;

    /**
     * @brief Retrieves a pointer to the immutable bound term
     * @return An immutable pointer to the bound term
     */
    [[nodiscard]] const IProcessedTerm *observe_bound_term() const noexcept;

    /**
     * @brief Retrieves a pointer to the immutable bound sentence
     * @return An immutable pointer to the bound sentence
     */
    [[nodiscard]] const ISentence *observe_sentence() const noexcept;

    /**
     * @brief Hash any quantified IR node from a static context
     * @param quantifier_type The quantifier type to hash
     * @param bound_term An observing pointer to the bound term to hash
     * @param sentence An observing pointer to the bound sentence to hash
     * @param is_positive Is the instantiation of the node in a positive polarity?
     * @return Numeric hash of the quantified node described by the given parameters
     */
    [[nodiscard]] static std::size_t hash_quantified(
            QuantifierTypes quantifier_type, const ITerm *bound_term, const ISentence *sentence, bool is_positive);

    /**
     * @brief Serialise any type of quantified IR node from a static context into an output stream
     * @param ostream Destination output stream
     * @param quantifier_type The type of quantifier to serialise
     * @param bound_term An observing pointer to the bound term to serialise
     * @param sentence An observing pointer to the bound sentence to serialise
     * @param is_positive Is the instantiation of the node in a positive polarity?
     * @return The populated destination output stream
     */
    static std::ostream &serialise_quantified(std::ostream &ostream, QuantifierTypes quantifier_type,
            const ITerm *bound_term, const ISentence *sentence, bool is_positive);

    /**
     * @brief Maps a quantifier type to a human-readable string suitable for serialisation
     * @param type Type of quantifier to serialise
     * @return Serialised string for the given quantifier type
     */
    [[nodiscard]] static const char *get_operator_symbol(QuantifierTypes type);

private:
    const QuantifierTypes quantifier_type;
    const IProcessedTerm *const bound_term;
    const ISentence *const sentence;
    const bool is_positive;
};

} // namespace optifol

#endif
