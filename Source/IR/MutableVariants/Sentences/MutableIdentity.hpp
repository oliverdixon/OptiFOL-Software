/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Identity Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEIDENTITY_HPP
#define MUTABLEIDENTITY_HPP

#include <memory>

#include "../OwningBuildable.hpp"
#include "../Terms/IMutableTerm.hpp"
#include "IMutableSentence.hpp"

namespace optifol
{

/**
 * @class MutableIdentity
 * @brief A MutableIdentity IR node denotes an equivalence through equality between two separate ITerm IR nodes.
 *  Metadata and operands are mutable; operands are owned with @ref std::unique_ptr containers by the node.
 * @see Identity for the non-owning, immutable dual
 */
class MutableIdentity : public IMutableSentence,
                        public OwningBuildable<MutableIdentity>
{
public:
    /**
     * @brief Create a new mutable Identity IR node with the given LHS, RHS, and polarity.
     * @param lhs The LHS term container to be transferred to node ownership
     * @param rhs The RHS term container to be transferred to node ownership
     * @param is_positive Should the node be instantiated in a positive polarity?
     */
    [[maybe_unused]] MutableIdentity(
            std::unique_ptr<IMutableTerm> &&lhs, std::unique_ptr<IMutableTerm> &&rhs, bool is_positive = true);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    const ISentence *accept(RepositoryBuildingVisitor &visitor) override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    /**
     * @brief Steal ownership of the LHS term from the node
     * @return The LHS term container
     * @warning This call transfers ownership outbound
     */
    [[nodiscard]] std::unique_ptr<IMutableTerm> take_lhs_operand() noexcept;

    /**
     * @brief Steal ownership of the RHS term from the node
     * @return The container containing the RHS term
     * @warning This call transfers ownership outbound
     */
    [[nodiscard]] std::unique_ptr<IMutableTerm> take_rhs_operand() noexcept;

    /**
     * @brief Retrieves an immutable observing pointer to the LHS term operand
     * @return The LHS term operand pointer
     * @see @ref std::unique_ptr::get for semantics of the observing getter
     */
    [[nodiscard]] const IMutableTerm *observe_lhs_operand() const noexcept;

    /**
     * @brief Retrieves an immutable observing pointer to the RHS term operand
     * @return The RHS term operand pointer
     * @see @ref std::unique_ptr::get for semantics of the observing getter
     */
    [[nodiscard]] const IMutableTerm *observe_rhs_operand() const noexcept;

    /**
     * @brief Transfers ownership of the given LHS term container to the node
     * @param new_lhs The new LHS term container
     * @warning This call transfers ownership inbound
     * @see @ref std::unique_ptr::operator= for semantics of swap
     */
    void put_lhs_operand(std::unique_ptr<IMutableTerm> &&new_lhs) noexcept;

    /**
     * @brief Transfers ownership of the given RHS term container to the node
     * @param new_rhs The new RHS term container
     * @warning This call transfers ownership inbound
     * @see @ref std::unique_ptr::operator= for semantics of swap
     */
    void put_rhs_operand(std::unique_ptr<IMutableTerm> &&new_rhs) noexcept;

private:
    std::unique_ptr<IMutableTerm> lhs;
    std::unique_ptr<IMutableTerm> rhs;
    bool is_positive;
};

} // namespace optifol

#endif
