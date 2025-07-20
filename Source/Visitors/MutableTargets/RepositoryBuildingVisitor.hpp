/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Symbol Repository-Building Visitor
 * @author Oliver Dixon
 * @date 2025-06-22
 * @version Development
 */

#ifndef REPOSITORYBUILDINGVISITOR_HPP
#define REPOSITORYBUILDINGVISITOR_HPP

#include <string_view>

#include "../../IR/SymbolRepository.hpp"

namespace optifol
{

class MutablePredicate;
class Predicate;
class MutableQuantified;
class Quantified;
class MutableSentenceRoot;
class SentenceRoot;
class MutableBinaryConnected;
class BinaryConnected;
class MutableIdentity;
class Identity;
class MutableVariable;
class Variable;
class MutableFunction;
class Function;
class MutableConstant;
class Constant;

/**
 * @class RepositoryBuildingVisitor
 * @brief Recursively visits a mutable, owning IMutableSentence/IMutableTerm AST structure and extracts common terms
 *  into a baseline SymbolRepository; transforms into an immutable, non-owning ISentence/IProcessedTerm AST.
 * @details
 *  <p>
 *      Logical analysis requires identification of common symbols (predicates, functions, free variables etc.)
 *      across all formulas within a problem set. Lexing, parsing, and CNF normalisation does not handle extraction of
 *      symbols, and the handling of duplicates thereof, into a common domain repository. The RepositoryBuildingVisitor
 *      performs this task as the last stage of the FOL-preparation pipeline. HCI is also substantially improved as
 *      common elements can be identified and manipulated with visual aids.
 *  </p>
 *  <p>
 *      The translation of a mutable, owning AST tree into an immutable, non-owning dual achieves large gains in
 *      expressiveness and simplicity. Ownership semantics are reduced to a single owning repository (quickly searchable
 *      due to hashing properties of immutable nodes and STL @ref std::unordered_set with C++23 transparent hashing)
 *      that imposes lifetime guarantees on its members. Immutable AST nodes then hold an observing raw pointer to the
 *      corresponding item detained by the SymbolRepository, accessible through const noexcept getters, which are
 *      aliased in cases of common symbols across nodes and formulas.
 *  </p>
 *  <p>
 *      In particular, the visitor performs the following transformations on the mutable sentence trees to produce
 *      corresponding immutable trees:
 *      <table>
 *          <tr>
 *              <th>Mutable Sentence Target</th>
 *              <th>Immutable Production</th>
 *              <th>Extractable Terms</th>
 *              <th>Extractable Sentences</th>
 *          </tr>
 *          <tr>
 *              <td>MutableQuantified</td>
 *              <td>Quantified</td>
 *              <td>Bound variable</td>
 *              <td>Bound sentence</td>
 *          </tr>
 *          <tr>
 *              <td>MutableBinaryConnected</td>
 *              <td>BinaryConnected</td>
 *              <td>None</td>
 *              <td>LHS sentence; RHS sentence</td>
 *          </tr>
 *          <tr>
 *              <td>MutableIdentity</td>
 *              <td>Identity</td>
 *              <td>LHS term; RHS term</td>
 *              <td>None</td>
 *          </tr>
 *          <tr>
 *              <td>MutablePredicate</td>
 *              <td>Predicate</td>
 *              <td>All argument terms</td>
 *              <td>None</td>
 *          </tr>
 *          <tr>
 *              <td>MutableSentenceRoot</td>
 *              <td>SentenceRoot with ownership responsibility</td>
 *              <td>None</td>
 *              <td>Detained sentence</td>
 *          </tr>
 *      </table>
 *      Note that the MutableSentenceRoot transformation represents a special case: the generated SentenceRoot is not
 *      registered in a SymbolRepository but instead passed within an ownership-controlled container, which becomes the
 *      responsibility of the visitor. Callers may then extract the last-generated sentence root from the visitor
 *      instance.
 *  </p>
 *  <p>
 *      Term trees are also eligible for transformation by the visitor:
 *      <table>
 *          <tr>
 *              <th>Mutable Term Target</th>
 *              <th>Immutable Production</th>
 *              <th>Extractable Terms</th>
 *          </tr>
 *          <tr>
 *              <td>MutableConstant</td>
 *              <td>Constant</td>
 *              <td>None</td>
 *          </tr>
 *          <tr>
 *              <td>MutableFunction</td>
 *              <td>Function</td>
 *              <td>All argument terms</td>
 *          </tr>
 *          <tr>
 *              <td>MutableVariable</td>
 *              <td>Variable</td>
 *              <td>None</td>
 *          </tr>
 *      </table>
 *  </p>
 * @see SymbolRepository
 * @warning The RepositoryBuildingVisitor is a one-way transformation between mutable and immutable representations.
 *  Visited nodes are always decomposed and rendered unusable to transfer ownership to the central SymbolRepository.
 */
class RepositoryBuildingVisitor final
{
public:
    /**
     * @brief Create a RepositoryBuildingVisitor with a corresponding SymbolRepository for trans-formula symbol storage
     * @param symbol_repository A mutable SymbolRepository baseline with a lifetime guaranteed to extend that of the
     *  visitor.
     */
    explicit RepositoryBuildingVisitor(SymbolRepository &symbol_repository);

    /**
     * @brief Create a new RepositoryBuildingVisitor templated from the current object, using the same global symbol
     *  repository reference but discarding any transitory state.
     * @return The constructed visitor instance.
     */
    [[nodiscard]] RepositoryBuildingVisitor clone_from_template() const;

    /**
     * @brief Gets the human-readable visitor name
     * @return An observing view of the human-readable visitor name string
     */
    [[nodiscard]] static std::string_view get_visitor_name();

    [[nodiscard]] const Quantified *visit(MutableQuantified &node);

    [[nodiscard]] const BinaryConnected *visit(MutableBinaryConnected &node);

    [[nodiscard]] const Identity *visit(MutableIdentity &node);

    [[nodiscard]] const Predicate *visit(MutablePredicate &node);

    [[nodiscard]] const Variable *visit(const MutableVariable &node) const;

    [[nodiscard]] const Function *visit(MutableFunction& node);

    [[nodiscard]] const Constant *visit(const MutableConstant & node) const;

    void visit(MutableSentenceRoot &node);

    std::unique_ptr<SentenceRoot> take_last_root() noexcept;

private:
    std::unique_ptr<SentenceRoot> root;

    static const char *visitor_name;

    SymbolRepository &symbol_repository;
};

} // namespace optifol

#endif
