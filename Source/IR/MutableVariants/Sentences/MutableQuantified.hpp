/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Quantified Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEQUANTIFIED_HPP
#define MUTABLEQUANTIFIED_HPP

#include <memory>

#include "../../MutableVariants/Terms/IMutableTerm.hpp"
#include "../OwningBuildable.hpp"
#include "IMutableSentence.hpp"

namespace optifol
{

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
 * @class MutableQuantified
 * @brief A MutableQuantified IR node owns a bound variable/term and a bound sentence ("operands"), and detains the
 *  first-order logic operator by which the sentence is bound by the variable. Metadata and operands are mutable, and
 *  owned by the instance of the IR node.
 * @note There is no immutable, non-owning dual of the MutableQuantified node as it may not appear in CNF-normalised
 *  expression trees.
 */
class MutableQuantified : public IMutableSentence,
                          public OwningBuildable<MutableQuantified>
{
public:
    [[maybe_unused]] MutableQuantified(QuantifierTypes quantifier_type, std::unique_ptr<IMutableTerm> &&bound_term,
            std::unique_ptr<IMutableSentence> &&sentence, bool is_positive = true);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingNodeVisitor &visitor) const override;

    const IProcessedSentence *accept(RepositoryBuildingVisitor &visitor) override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] bool operator==(const IMutableSentence &other) const noexcept override;

    /**
     * @brief Replaces the quantifier operator with an alternative type
     * @param quantifier_type The new quantifier operator
     */
    void set_quantifier_type(QuantifierTypes quantifier_type) noexcept;

    /**
     * @brief Retrieve the quantification operator that binds the variable and the sentence
     * @return The quantifier type
     */
    [[nodiscard]] QuantifierTypes get_quantifier_type() const noexcept;

    /**
     * @brief Retrieves a pointer to the immutable bound term
     * @return An immutable pointer to the bound term
     * @see @ref std::unique_ptr::get for semantics of the observing getter
     */
    [[nodiscard]] const IMutableTerm *observe_bound_term() const noexcept;

    /**
     * @brief Steals ownership of the bound term from the object to the caller
     * @return The stolen container containing the bound term
     * @warning This call transfers ownership outbound
     */
    [[nodiscard]] std::unique_ptr<IMutableTerm> take_bound_term() noexcept;

    /**
     * @brief Steals ownership of the bound sentence from the object to the caller
     * @return The stolen container containing the bound sentence
     * @warning This call transfers ownership outbound
     */
    [[nodiscard]] std::unique_ptr<IMutableSentence> take_sentence() noexcept;

    /**
     * @brief Retrieves a pointer to the immutable bound sentence
     * @return An immutable pointer to the bound sentence
     * @see @ref std::unique_ptr::get for semantics of the observing getter
     */
    [[nodiscard]] const IMutableSentence *observe_sentence() const noexcept;

    /**
     * @brief Transfers ownership of a new bound sentence, overwriting any previously bound sentence
     * @param sentence The new sentence-owning container
     * @warning This call transfers ownership inbound
     * @see @ref std::unique_ptr::operator= for semantics of swap
     */
    void put_sentence(std::unique_ptr<IMutableSentence> &&sentence) noexcept;

    /**
     * @brief Transfers ownership of a new bound term, overwriting any previously bound term
     * @param new_bound_term The new term-owning container
     * @warning This call transfers ownership inbound
     * @see @ref std::unique_ptr::operator= for semantics of swap
     */
    void put_bound_term(std::unique_ptr<IMutableTerm> &&new_bound_term) noexcept;

    /**
     * @brief Maps a quantifier type to a human-readable string suitable for serialisation
     * @param type Type of quantifier to serialise
     * @return Serialised string for the given quantifier type
     */
    [[nodiscard]] static const char *get_operator_symbol(QuantifierTypes type);

private:
    QuantifierTypes quantifier_type;
    std::unique_ptr<IMutableTerm> bound_term;
    std::unique_ptr<IMutableSentence> sentence;
    bool is_positive;
};

} // namespace optifol

#endif
