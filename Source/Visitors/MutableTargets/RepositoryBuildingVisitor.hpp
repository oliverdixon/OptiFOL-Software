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
class MutableQuantified;
class MutableSentenceRoot;
class MutableBinaryConnected;
class MutableIdentity;
class IMutableTerm;

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
 *      </table>
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
 *  <p>
 *      Note that other visitor/accept pairs are provided, such as for mutable-only types (see MutableSentenceRoot)
 *      solely for consistently on the visitor interface. These member functions are guaranteed to be no-ops and are
 *      marked as such with the <code>static</code> keyword.
 *  </p>
 * @see SymbolRepository
 */
class RepositoryBuildingVisitor final
{
public:
    /**
     * @brief Create a RepositoryBuildingVisitor with a corresponding SymbolRepository for trans-formula symbol storage
     * @param symbol_repository A mutable SymbolRepository baseline with a lifetime guaranteed to extend that of the
     *  visitor.
     */
    explicit RepositoryBuildingVisitor(SymbolRepository& symbol_repository);

    /**
     * @brief Gets the human-readable visitor name
     * @return An observing view of the human-readable visitor name string
     */
    [[nodiscard]] static std::string_view get_visitor_name();

    void visit(MutableQuantified& node);

    void visit(MutableBinaryConnected& node);

    void visit(MutableIdentity& node);

    void visit(MutablePredicate& node);

    void visit(MutableSentenceRoot& node);

    void visit(IMutableTerm& node);

private:
    static const char * visitor_name;

    SymbolRepository& symbol_repository;
};

} // namespace optifol

#endif // REPOSITORYBUILDINGVISITOR_HPP
