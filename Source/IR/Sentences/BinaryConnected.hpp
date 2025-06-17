/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the binary-Connected Sentence IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#ifndef BINARYCONNECTED_HPP
#define BINARYCONNECTED_HPP

#include "ISentence.hpp"

namespace optifol
{

/**
 * @enum BinaryOperatorTypes
 * @brief Types of operators usable by binary-connected sentences to connect two operands
 */
enum class BinaryOperatorTypes
{
    Conjunction,
    Disjunction,
    Implication,
    Biconditional,
};

/**
 * @class BinaryConnected
 * @brief A BinaryConnected IR node references two operands semantically joined with an operator. Metadata and operands
 *  are immutable; operands are held centrally in a SymbolRepository.
 * @see BinaryOperatorTypes for modes of connection
 * @see MutableBinaryConnected for the owning dual
 */
class BinaryConnected :
        public ISentence
{
public:
    /**
     * @brief Create a signed binary-connected IR node
     * @param operator_type Operator with which the operands are connected
     * @param lhs The left-hand operand reference in the SymbolRepository
     * @param rhs The right-hand operand reference in the SymbolRepository
     * @param is_positive Should the node be instantiated with in a positive polarity?
     */
    explicit BinaryConnected(BinaryOperatorTypes operator_type, const ISentence * lhs, const ISentence * rhs,
        bool is_positive = true);

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream& serialise(std::ostream &ostream) const override;

    /**
     * @brief Get the fixed operator by which the operands are connected
     * @return The binary-connected operator type
     */
    [[nodiscard]] BinaryOperatorTypes get_operator_type() const noexcept;

    /**
     * @brief Provides an immutable observing pointer to the owned LHS operand
     * @return The LHS operand observer
     */
    [[nodiscard]] const ISentence * get_lhs_operand() const noexcept;

    /**
     * @brief Provides an immutable observing pointer to the owned RHS operand
     * @return The RHS operand observer
     */
    [[nodiscard]] const ISentence * get_rhs_operand() const noexcept;

    /**
     * @brief Hash any type of binary-connected IR node from a static context
     * @param operator_type The type of binary operator with which the operands are connected
     * @param lhs An observing pointer to the LHS operand
     * @param rhs An observing pointer to the RHS operand
     * @param is_negative_polarity Has the target been instantiated with a negative polarity?
     * @return Numeric hash of the binary-connected node described by the given parameters
     */
    [[nodiscard]] static std::size_t hash_binary_connected(BinaryOperatorTypes operator_type, const ISentence *lhs,
                                             const ISentence *rhs, bool is_negative_polarity) noexcept;


    /**
     * @brief Serialise any type of binary-connected IR node from a static context into an output stream
     * @param ostream Destination output stream
     * @param operator_type The type of binary operator with which the operands are connected
     * @param lhs An observing pointer to the LHS operand
     * @param rhs An observing pointer to the RHS operand
     * @param is_negative_polarity Has the target been instantiated with a negative polarity?
     * @return Populated destination output stream
     */
    static std::ostream& serialise_binary_connected(std::ostream &ostream, BinaryOperatorTypes operator_type,
                                                    const ISentence *lhs, const ISentence *rhs,
                                                    bool is_negative_polarity);

    /**
     * @brief Return the appropriate human-readable symbol for a BinaryOperatorTypes value
     * @param type The serialisation target
     * @return A human-readable string representation of a binary operator
     */
    [[nodiscard]] static const char *get_operator_symbol(BinaryOperatorTypes type);

private:
    const BinaryOperatorTypes operator_type;

    const ISentence * const lhs;

    const ISentence * const rhs;

    const bool is_positive;
};

}

#endif
