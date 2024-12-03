/**
 * @file SymbolStandardisingVisitor.hpp
 * @brief Class specification for the Symbol-Standardising Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-12-01
 * @version Development
 */

#ifndef OPTIFOL_SYMBOLSTANDARDISINGVISITOR_HPP
#define OPTIFOL_SYMBOLSTANDARDISINGVISITOR_HPP

#include <unordered_map>

#include "../MutatingSentenceVisitorBase.hpp"
#include "../../Terms/TermResolutionVisitor.hpp"

namespace optifol
{

/**
 * @class SymbolStandardisingVisitor
 * @brief The symbol-standardising visitor validates the semantic correctness of variables and standardises the names
 *  thereof throughout the model.
 *
 * @details The symbol-standardising visitor recurses through the model, paying particular attention to variables. It
 *  performs two overarching tasks:
 *  <ol>
 *    <li><b>Verifying semantic correctness:</b> enforces the rules surrounding the usage of variables. In particular,
 *      variables may only be introduced by way of a quantifier. If a variable is referenced without having been
 *      bound by a quantifier in the relevant scope, an exception is thrown. Dually, if a variable is bound by a
 *      quantifier having already been defined in the relevant scope, an exception is thrown.</li>
 *    <li><b>Standardising naming of adjacent variables:</b> ensures that syntactically and semantically valid sentences
 *      discriminate between variables of the same name when used across different scopes. For example,
 *      <code>%Ux(P(x)) | %Ex(Q(x))</code> would be suitably rewritten as <code>%Ux(P(x)) | %Ex0(Q(x0))</code>.</li>
 *  </ol>
 *
 * @warning Although multiple passes are not required for this symbol-standardising visitor, it does recurse on any
 *  produced terms to ensure a full reduction. On extremely deeply nested sentences, this could cause a machine stack
 *  overflow.
 *
 * @throws SemanticException
 */
class SymbolStandardisingVisitor :
        public MutatingSentenceVisitorBase
{
public:
    /**
     * @brief Recursively applies the symbol-standardisation procedures to the given quantified sentence root.
     * @param node The root quantified sentence node
     */
    void visit(QuantifiedSentenceNode &node) override;

    /**
     * @brief Recursively applies the symbol-standardisation procedures to the given predicate root node.
     * @param node The root predicate sentence node
     */
    void visit(PredicationNode &node) override;

    /**
     * @brief Recursively applies the symbol-standardisation procedures to the given identity sentence node.
     * @param node The root identity sentence node
     */
    void visit(IdentitySentenceNode &node) override;

    void reset() override;

private:
    /**
     * @brief Name-rewriting rules for variables encountered in the current scope.
     * @details Any variable with a name matching a key of the map should be completely replaced by the variable shared
     *  pointer in the corresponding value. Only variables with disambiguated names occupy entries in the map, and the
     *  map should be cleared down when the scope is released.
     */
    std::unordered_map<std::string, std::shared_ptr<VariableNode>> rewriting_rules;

    /**
     * @brief The set of pre-disambiguated names bound in the current scope.
     */
    std::unordered_set<std::string> scope;

    /**
     * @brief The set of pre-disambiguated names bound by adjacent scopes, not including the current scope.
     */
    std::unordered_set<std::string> adjacent;

    /**
     * @brief The suffix of the next variable name requiring disambiguation.
     */
    unsigned int unique_name_counter = 0;

    /**
     * @brief The nested term visitor used to assist rewriting of variables nested in terms that are not accessible
     *  through the sentence interface (i.e. functions).
     */
    TermResolutionVisitor term_visitor{scope, rewriting_rules};

    /**
     * @brief Establishes a new scope, introducing the variable bound by the given quantifier. If necessary, the bound
     *  name is disambiguated, in which case a rewriting rule entry is added.
     * @param node The bounding quantifier responsible for the opened scope
     */
    void open_scope(QuantifiedSentenceNode& node);

    /**
     * @brief Closes the current scope, clearing applicable entries from the scope naming set and the rewriting rule
     *  map. The bound variable of the closed scope is comitted to the adjacents naming set.
     * @pre The scope naming set must contain a variable name of the given node.
     * @param node The variable bound by the scope
     */
    void close_scope(const VariableNode& node);

    /**
     * @brief Suffix the given variable name with a unique identifier, until it does not conflict with any member of the
     *  adjacents naming set.
     * @param name The ambiguous name
     * @return The disambiguated name
     */
    std::string generate_name(const std::string &name);
};

}

#endif //OPTIFOL_SYMBOLSTANDARDISINGVISITOR_HPP
