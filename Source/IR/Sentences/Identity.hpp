/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Identity Sentence IR node
 * @author Oliver Dixon
 * @date 2025-06-18
 * @version Development
 */

#ifndef IDENTITY_HPP
#define IDENTITY_HPP

#include "IProcessedSentence.hpp"

namespace optifol
{

class IProcessedTerm;
class ITerm;

/**
 * @class Identity
 * @brief An Identity IR node denotes an equivalence through equality between two separate IProcessedTerm IR nodes.
 *  Metadata and operands are immutable; operands are expected to be centrally managed by a SymbolRepository.
 * @see MutableIdentity for the owning, mutable dual
 */
class Identity : public IProcessedSentence
{
public:
    /**
     * @brief Create a new immutable Identity IR node with the given LHS, RHS, and polarity.
     * @param lhs The LHS term operand of the Identity
     * @param rhs The RHS term operand of the Identity
     * @param is_positive Should the node be instantiated in a positive polarity?
     */
    Identity(const IProcessedTerm * lhs, const IProcessedTerm * rhs, bool is_positive = true);

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    /**
     * @brief Retrieve an observing pointer to the LHS term operand
     * @return An observing pointer to the LHS of the Identity IR node
     */
    [[nodiscard]] const IProcessedTerm * observe_lhs_operand() const noexcept;

    /**
     * @brief Retrieve an observing pointer to the RHS term operand
     * @return An observing pointer to the RHS of the Identity IR node
     */
    [[nodiscard]] const IProcessedTerm * observe_rhs_operand() const noexcept;

    /**
     * @brief Hash an identity IR node from a static context
     * @param lhs An observing pointer to the LHS operand
     * @param rhs An observing pointer to the RHS operand
     * @param is_negative_polarity Has the target been instantiated with a negative polarity?
     * @return Numeric hash of the identity IR node described by the given parameters
     */
    [[nodiscard]] static std::size_t hash_identity(
            const ITerm *lhs, const ITerm *rhs, bool is_negative_polarity) noexcept;

    /**
     * @brief Serialise an identity IR node from a static context
     * @param ostream Destination output stream
     * @param lhs An observing pointer to the LHS operand
     * @param rhs An observing pointer to the RHS operand
     * @param is_negative_polarity Has the target been instantiated with a negative polarity?
     * @return Populated destination output stream
     */
    static std::ostream& serialise_identity(
            std::ostream &ostream, const ITerm *lhs, const ITerm *rhs, bool is_negative_polarity);

private:
    const IProcessedTerm * const lhs;
    const IProcessedTerm * const rhs;
    const bool is_positive;
};

} // namespace optifol

#endif // IDENTITY_HPP
