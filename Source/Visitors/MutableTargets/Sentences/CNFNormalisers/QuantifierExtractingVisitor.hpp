/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Quantifier Extracting Visitor
 * @author Oliver Dixon
 * @date 2025-05-04
 * @version Development
 */

#ifndef QUANTIFIEREXTRACTINGVISITOR_HPP
#define QUANTIFIEREXTRACTINGVISITOR_HPP

#include <memory>
#include <optional>

#include "../../../../IR/MutableVariants/Sentences/IMutableSentence.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutableQuantified.hpp"
#include "../../../../IR/MutableVariants/Terms/IMutableTerm.hpp"
#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

/**
 * @class QuantifierExtractingVisitor
 * @brief The quantifier-extracting visitor shifts quantifiers outwards of binary-connected nodes.
 *
 * @details
 *  <p>
 *      The quantifier-extracting visitor rewrites the model such that quantifiers may only occur in the
 * initial prefix of a formula, but never inside a negation, conjunction, or disjunction. In particular, the
 * following transformations are repeatedly applied: <table> <tr> <th>#</th> <th>Match</th>
 *              <th>Replacement</th>
 *          </tr>
 *          <tr>
 *              <td>1</td>
 *              <td>@f$ P \land \forall x Q(x) @f$</td>
 *              <td>@f$ \forall x \left( P \land Q(x) \right) @f$</td>
 *          </tr>
 *          <tr>
 *              <td>2</td>
 *              <td>@f$ P \lor \forall x Q(x) @f$</td>
 *              <td>@f$ \forall x \left( P \lor Q(x) \right) @f$</td>
 *          </tr>
 *          <tr>
 *              <td>3</td>
 *              <td>@f$ P \land \exists x Q(x) @f$</td>
 *              <td>@f$ \exists x \left( P \land Q(x) \right) @f$</td>
 *          </tr>
 *          <tr>
 *              <td>4</td>
 *              <td>@f$ P \lor \exists x Q(x) @f$</td>
 *              <td>@f$ \exists x \left( P \lor Q(x) \right) @f$</td>
 *          </tr>
 *      </table>
 *  </p>
 *  <p>
 *      Note that the above transformations are commutative in the binary operator. That is, the above
 * transformations match equally for quantifiers on the LHS and non-quantifiers on the RHS. Cruically, binary
 * sentences with quantifiers in both or neither operand slot(s) are not eligible for transformation.
 *  </p>
 */
class QuantifierExtractingVisitor : public MutatingSentenceVisitorBase
{
public:
    [[nodiscard]] std::string_view get_visitor_name() const override;

    void visit(MutableBinaryConnected &node) override;

    void visit(MutableQuantified &node) override;

    void visit(MutableSentenceRoot &node) override;

private:
    /**
     * @class QuantifiedTemplate
     * @brief A basic owning structure for quantifier data (type, bound term, and bound sentence) with an
     * owner to which it can be returned.
     */
    struct QuantifiedTemplate
    {
        /**
         * @brief Construct a new QuantifiedTemplate to hold ownership of the given bound term and sentence,
         * borrowed from the specified owner
         * @param type The characteristic type of the quantifier
         * @param bound_term The variable term bound by the quantifier, to be owned by the template
         * @param sentence The sentence bound by the quantifier, to be owned by the template
         * @param owner The owner of the bound term and sentence to which responsibility may be returned at
         * any point throughout the lifetime of the QuantifiedTemplate.
         */
        QuantifiedTemplate(const QuantifierTypes type, std::unique_ptr<IMutableTerm> &&bound_term,
                std::unique_ptr<IMutableSentence> &&sentence, MutableQuantified *owner) :
            type(type),
            bound_term(std::move(bound_term)),
            sentence(std::move(sentence)),
            owner(owner)
        {
        }

        /**
         * @brief Return the borrowed elements to the responsibility of the original stated owner
         */
        void return_to_owner()
        {
            owner->put_sentence(std::move(sentence));
            owner->put_bound_term(std::move(bound_term));
        }

        QuantifierTypes type;
        std::unique_ptr<IMutableTerm> bound_term;
        std::unique_ptr<IMutableSentence> sentence;

    private:
        MutableQuantified *const owner;
    };

    /**
     * @enum TrackingMode
     * @brief Indicate the current state of 'tracking', as required by a calling visitor.
     * @details When tracking is enabled (left- or right-major), operands/children of binary-connected nodes
     * should be tracked by the QuantifierExtractingVisitor instance.
     */
    enum class TrackingMode
    {
        NotTracking, /**< Not tracking; nested children shouldn't record their operands. */
        LeftMajor, /**< Tracking to the left: nested children should record their left operands in the major
                      slot */
        RightMajor /**< Tracking to the right: nested children should record their right operands in the major
                      slot */
    };

    /**
     * @brief Apply any pending quantification transformation to the given (borrowed) target
     * @param transform_target The target sentence to be transformed/wrapped by the pending transformation
     * quantifier
     * @return An owning container to the wrapped target sentence, or the original sentence if there was no
     * suitable pending transform.
     * @details There are two possible cases:
     *  <ol>
     *      <li>
     *          There is no pending transformation. This function does nothing, and returns an owning
     * container to the borrowed target.
     *      </li>
     *      <li>
     *          <p>
     *              There is a pending transformation encoding a type and bound term. The transformation
     * target is interpreted as a sentence that should be wrapped by a MutableQuantifierNode with the
     * character and bound variable described by the transformation pair. The transformation is applied to the
     *              target, thus making it a MutableQuantifierNode binding the sentence previously held by the
     *              target, and the owning container is returned.
     *          </p>
     *          <p>
     *              In general, if the transformation metadata holds a <code>T</code>-type quantifier binding
     * the variable <code>x</code>, and the transformation target holds a sentence <code>S</code>, the
     *              returned value is an owning container for a <code>T</code>-type quantifier binding the
     * variable <code>x</code> with the sentence <code>S</code>.
     *          </p>
     *      </li>
     *  </ol>
     */
    std::unique_ptr<IMutableSentence> apply_transform(std::unique_ptr<IMutableSentence> &&transform_target);

    static const char *visitor_name;

    TrackingMode tracking_mode = TrackingMode::NotTracking;
    std::optional<QuantifiedTemplate> quant_lhs_data;
    std::optional<QuantifiedTemplate> quant_rhs_data;
    std::optional<std::pair<QuantifierTypes, std::unique_ptr<IMutableTerm>>> transformation;
};

} // namespace optifol

#endif
