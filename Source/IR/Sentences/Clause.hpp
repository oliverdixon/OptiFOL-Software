/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Clause collection
 * @author Oliver Dixon
 * @date 2025-09-15
 * @version Development
 */

#ifndef OPTIFOL_CLAUSE_HPP
#define OPTIFOL_CLAUSE_HPP

#include <vector>

#include "Literal.hpp"

namespace optifol
{

class Literal;

/**
 * @class Clause
 * @brief A Clause is an unordered set of Literal nodes considered under disjunction.
 * @details
 *  <p>
 *      An @f$ N @f$-Clause @f$ C @f$ is represented in FOL as
 *      @f[ C = \left\{ L_1, \ldots, L_N \right\} = L_1 \lor \ldots \lor L_N @f] for Literal nodes
 *      @f$ L_1, \ldots, L_N @f$. Under standard FOL rules, a Clause is trivially false ("bottom") if
 *      @f$ C = \emptyset @f$, and likewise trivially true ("a tautology") if
 *      @f$ \exists L_i \in C \left( \lnot L_i \in C \right) @f$.
 *  </p>
 *  <p>
 *      This implementation models a tautology with @f$ C := \emptyset @f$. Trivial states can be further queried.
 *  </p>
 */
class Clause
{
public:
    /**
     * @enum State
     * @brief Triviality state of the Clause, indicating a bottom (unsatisfiable) or tautology (trivially satisfiable).
     */
    enum class State
    {
        NotTrivial,
        TriviallyTrue,
        TriviallyFalse
    };

private:
    /**
     * @brief The collection of Literal objects
     * @note An ordering according to Literal::operator<(const Literal&) is currently enforced on this container by
     *  @ref Clause::Clause() and Clause::add_literal. Mathematically, a Clause is unordered due to the associativity of
     *  FOL disjunction, but it's easier to keep an ordering here to detect tautologies or duplicate entries.
     */
    std::vector<const Literal *> literals;

    State state = State::TriviallyFalse;

public:
    /**
     * @brief Construct a new trivially false Clause.
     */
    Clause() = default;

    /**
     * @brief Construct a new Clause with an initial set of Literals
     * @param literals The initial set of Literals to be considered for addition to the Clause
     */
    explicit Clause(std::initializer_list<const Literal *> literals);

    /**
     * @brief Add a new Literal to the Clause store, subject to conditions.
     * @details Mutating the state of the Clause can change its structure. In particular:
     *  <ul>
     *      <li>If the same Literal is already present, it is silently rejected.</li>
     *      <li>If the complentary Literal is already present, the Clause is deemed a tautology and has its store
     *          cleared and state updated accordingly.</li>
     *      <li>If none of the above, it is added. If the Clause was previously bottom, it is updated to
     *          non-trivial.</li>
     *  </ul>
     * @param new_literal The incoming Literal to consider adding.
     */
    void add_literal(const Literal * new_literal);

    /**
     * @brief Reports on the triviality state of the Clause, indicating whether it is a tautology.
     * @return Triviality state of the Clause.
     */
    [[nodiscard]] State get_triviality_state() const noexcept;

    /**
     * @brief Retrieve a constant iterator to the beginning of the underlying Literal storage container.
     * @return An iterator to the first Literal in the Clause.
     */
    [[nodiscard]] decltype(literals)::const_iterator begin() const noexcept;

    /**
     * @brief Retrieve a constant iterator to one-past-the-end of the underlying Literal storage container.
     * @return An iterator to one-past-the-end Literal in the Clause.
     */
    [[nodiscard]] decltype(literals)::const_iterator end() const noexcept;

    /**
     * @brief Serialise the Clause with all Literal objects to the given output stream.
     * @param ostream The destination output stream
     * @param clause The Clause to serialise
     * @return The populated output stream
     */
    friend std::ostream& operator<<(std::ostream& ostream, const Clause& clause);

    bool operator<(const Clause& other) const noexcept;

    /**
     * @brief Determine equality between two Clauses by zipping the Literal nodes
     * @param other The other Clause to test
     * @return Are the two Clauses equal?
     */
    [[nodiscard]] bool operator==(const Clause &other) const noexcept;

    /**
     * @brief Gets the size/order of the Clause.
     * @return The number of Literal objects in the Clause.
     */
    [[nodiscard]] std::size_t order() const noexcept;

    /**
     * @brief Is the Clause empty?
     * @return Is the Clause empty, or equivalently, trivially false?
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Is the Clause a unit clause?
     * @return Is the Clause a unit, or equivalently, of order 1?
     */
    [[nodiscard]] bool unit() const noexcept;

    /**
     * @brief Reset the Clause to its bottom state of order zero, where it is trivially unsatisfiable.
     */
    void force_bottom() noexcept;

private:
    /**
     * @brief Checks whether there exists a complementary pair of Literal objects in the Clause.
     * @return Is the Clause a tautology?
     * @pre The Literal objects are sorted in the canonical manner; see @ref literals.
     */
    [[nodiscard]] bool is_tautology() const noexcept;
};

} // namespace optifol

#endif // OPTIFOL_CLAUSE_HPP
