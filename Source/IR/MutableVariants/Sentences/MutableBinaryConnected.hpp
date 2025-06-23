/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable binary-Connected Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEBINARYCONNECTED_HPP
#define MUTABLEBINARYCONNECTED_HPP

#include <memory>

#include "../../Sentences/BinaryConnected.hpp"
#include "../OwningBuildable.hpp"
#include "IMutableSentence.hpp"

namespace optifol
{

/**
 * @class MutableBinaryConnected
 * @brief A MutableBinaryConnected IR node holds and owns two operands semantically joined with an operator.
 *  Metadata and operands are mutable.
 * @see BinaryOperatorTypes for modes of connection
 * @see BinaryConnected for non-owning dual
 */
class MutableBinaryConnected : public IMutableSentence,
                               public OwningBuildable<MutableBinaryConnected>
{
public:
    /**
     * @brief Create a signed mutable binary-connected IR node
     * @param operator_type Operator with which the operands are connected
     * @param lhs Transferred owning container of the left-hand operand
     * @param rhs Transferred owning container of the right-hand operand
     * @param is_positive Should the node be instantiated with in a positive polarity?
     */
    [[maybe_unused]] MutableBinaryConnected(BinaryOperatorTypes operator_type, std::unique_ptr<IMutableSentence> &&lhs,
            std::unique_ptr<IMutableSentence> &&rhs, bool is_positive = true);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    const ISentence *accept(RepositoryBuildingVisitor &visitor) override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    /**
     * @brief Get the current operator by which the operands are connected
     * @return The binary-connected operator type
     */
    [[nodiscard]] BinaryOperatorTypes get_operator_type() const noexcept;

    /**
     * @brief Replaces the binary operator
     * @param new_type The new binary operator
     */
    void set_operator_type(BinaryOperatorTypes new_type) noexcept;

    /**
     * @brief Steals ownership of the LHS operand from the object to the caller
     * @return The stolen container containing the LHS operand
     * @warning This call transfers ownership outbound
     */
    [[nodiscard]] std::unique_ptr<IMutableSentence> take_lhs_operand() noexcept;

    /**
     * @brief Steals ownership of the RHS operand from the object to the caller
     * @return The stolen container containing the RHS operand
     * @warning This call transfers ownership outbound
     */
    [[nodiscard]] std::unique_ptr<IMutableSentence> take_rhs_operand() noexcept;

    /**
     * @brief Provides an immutable observing pointer to the owned LHS operand
     * @return The LHS operand observer
     * @see @ref std::unique_ptr::get for semantics of observing getter
     */
    [[nodiscard]] const IMutableSentence *observe_lhs_operand() const noexcept;

    /**
     * @brief Provides an immutable observing pointer to the owned RHS operand
     * @return The LHS operand observer
     * @see @ref std::unique_ptr::get for semantics of observing getter
     */
    [[nodiscard]] const IMutableSentence *observe_rhs_operand() const noexcept;

    /**
     * @brief Transfers ownership of a new LHS operand, overwriting any previously held LHS operand
     * @param operand The new LHS operand owning container
     * @warning This call transfers ownership inbound
     * @see @ref std::unique_ptr::operator= for semantics of swap
     */
    void put_lhs_operand(std::unique_ptr<IMutableSentence> &&operand) noexcept;

    /**
     * @brief Transfers ownership of a new RHS operand, overwriting any previously held LHS operand
     * @param operand The new RHS operand owning container
     * @warning This call transfers ownership inbound
     * @see @ref std::unique_ptr::operator= for semantics of swap
     */
    void put_rhs_operand(std::unique_ptr<IMutableSentence> &&operand) noexcept;

private:
    BinaryOperatorTypes operator_type;
    std::unique_ptr<IMutableSentence> lhs;
    std::unique_ptr<IMutableSentence> rhs;
    bool is_positive;
};

} // namespace optifol

#endif
