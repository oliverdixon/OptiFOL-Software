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

#include "../../IR/Sentences/BinaryConnected.hpp"
#include "../../IR/Sentences/SentenceRoot.hpp"
#include "../../IR/SymbolRepository.hpp"

namespace optifol
{

class MutablePredicate;
class Literal;
class MutableQuantified;
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
class MutableSkolemFunction;
class SkolemFunction;

class IProcessedSentence;

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
 *              <td>SentenceRoot</td>
 *              <td>None</td>
 *              <td>Detained sentence</td>
 *          </tr>
 *      </table>
 *      Note that the MutableSentenceRoot transformation represents a special case: the generated SentenceRoot is not
 *      registered in a SymbolRepository but instead passed within an ownership-controlled container, which becomes the
 *      responsibility of the visitor. Callers may then extract the last-generated sentence root from the visitor
 *      instance, which contains a set of literals ("clauses") under conjunction.
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
     * @param symbol_repository A mutable SymbolRepository
     */
    explicit RepositoryBuildingVisitor(std::shared_ptr<SymbolRepository> symbol_repository);

    /**
     * @brief Gets the human-readable visitor name
     * @return An observing view of the human-readable visitor name string
     */
    [[nodiscard]] static std::string_view get_visitor_name();

    /**
     * @brief Placeholder visitor to build an immutable quantified item from a MutableQuantified node.
     * @param node The quantified node.
     * @warning
     *  <p>
     *      This method always fails with a bad assertion. Mutable sentences are obliged to accept visitation requests
     *      from the RepositoryBuildingVisitor, but at present, the repository only contains expression trees in full
     *      Conjunctive Normal Form (CNF). Predicate CNF does not permit quantifiers, and all quantifiers should have
     *      been removed by the following visitors in the normalisation pipeline:
     *      <ul>
     *        <li>Universal quantifiers: UniversalEliminationVisitor; and</li>
     *        <li>Existential quantifiers: SkolemIntroducingVisitor (where existentials are replaced with synthesised
     *          Skolem functions provided by MutableSkolemFunction).
     *      </ul>
     *  </p>
     * @return @ref std::nullptr_t
     */
    static const IProcessedSentence *visit(const MutableQuantified &node);

    /**
     * @brief Transforms a MutableBinaryConnected disjunctive or conjunctive node into an equivalent BinaryConnected
     *  node.
     * @param node The MutableBinaryConnected node to be destroyed and used to construct the BinaryConnected equivalent.
     * @return The BinaryConnected node, referencing symbols in the SymbolRepository equivalent to the operands of the
     *  original MutableBinaryConnected.
     * @pre The given MutableBinaryConnected node must be a disjunction or conjunction; the presence of any other
     *  operator indicates a non-normalised tree.
     */
    const BinaryConnected *visit(MutableBinaryConnected &node);

    /**
     * @brief Transforms a MutableIdentity node into an Identity, registering the operand terms in the central
     *  SymbolRepository. The Identity itself is also added to the central SymbolRepository.
     * @param node The MutableIdentity node to be transformed into the corresponding immutable representation.
     * @return A non-owning immutable pointer to the SymbolRepository Identity node.
     */
    const Identity *visit(MutableIdentity &node);

    /**
     * @brief Transforms a MutablePredicate node into a Literal, registering the argument terms in the central
     *  SymbolRepository. The Literal itself is also added to the central SymbolRepository.
     * @param node The MutablePredicate node to be transformed into the corresponding immutable representation.
     * @return A non-owning immutable pointer to the SymbolRepository Literal node.
     */
    const Literal *visit(MutablePredicate &node);

    const Variable *visit(const MutableVariable &node) const;

    const Function *visit(MutableFunction& node);

    const SkolemFunction *visit(const MutableSkolemFunction& node);

    void visit(MutableSentenceRoot &node);

    std::unique_ptr<SentenceRoot> take_last_root() noexcept;

private:
    void commit_working_clause();

    /**
     * @brief The working clause stores the working set of literals under disjunction for the current clause. The
     *  working clause should be committed to the SentenceRoot @ref root node once it has been fully populated.
     * @see SentenceRoot::commit_clause
     */
    Clause working_clause;

    std::unique_ptr<SentenceRoot> root;

    static const char *visitor_name;

    const std::shared_ptr<SymbolRepository> symbol_repository;
};

} // namespace optifol

#endif
