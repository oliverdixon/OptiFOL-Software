/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the SymbolRepository class
 * @author Oliver Dixon
 * @date 2025-06-13
 * @version Development
 */

#ifndef SYMBOLREPOSITORY_HPP
#define SYMBOLREPOSITORY_HPP

#include <memory>
#include <optional>
#include <unordered_set>

#include "../IHashable.hpp"
#include "Sentences/Predicate.hpp"
#include "Terms/ITerm.hpp"

namespace optifol
{

/**
 * @class SymbolRepository
 * @brief A SymbolRepository persists a single set of PredicateNodes and ITerms with guaranteed lifetimes. The
 *  repository can be populated and queried, exposing constant pointers, but elements cannot be removed or destructed
 *  until the entire repository is destructed.
 */
class SymbolRepository
{
public:
    /**
     * @brief Add a generic term to the repository
     * @param term A \ref std::unique_ptr wrapper of the ITerm whose ownership should be transferred.
     * @return A constant handle to the term in the repository
     * @throws std::runtime_error An equivalent term does not exist in the repository, and could not be added.
     * @note If the supplied term is hash-equal to an existing term held by the repository, the repository is unchanged.
     */
    const ITerm *add_symbol(std::unique_ptr<ITerm> &&term);

    /**
     * @brief Add a predicate to the repository
     * @param predicate A \ref std::unique_ptr wrapper of the Predicate whose ownership should be transferred.
     * @return A constant handle to the predicate in the repository
     * @throws std::runtime_error An equivalent predicate does not exist in the repository, and could not be added.
     * @note If the supplied predicate is hash-equal to an existing predicate held by the repository, the repository is
     *  unchanged.
     */
    const Predicate *add_symbol(std::unique_ptr<Predicate> &&predicate);

    /**
     * @brief Retrieves a handle to an immutable term symbol owned by the repository
     * @param term A hash-equal ITerm to the target term
     * @return A constant handle to the term, if a suitable match exists in the repository. Otherwise, an empty \ref
     *  std::optional.
     */
    std::optional<const ITerm *> get_symbol_handle(const ITerm &term) const;

  /**
   * @brief Retrieves a handle to an immutable predicate symbol owned by the repository
   * @param predicate A hash-equal Predicate to the target predicate
   * @return A constant handle to the predicate, if a suitable match exists in the repository. Otherwise, an empty \ref
   *  std::optional.
   */
    std::optional<const Predicate *> get_symbol_handle(const Predicate &predicate) const;

private:
    /*
     * Note that we verify the satisfaction of Predicate and ITerms against the TransparentlyHashable concept
     * here in the class definition to produce readable error messages. If the hasher and equality functor call
     * operators cannot participate in overload resolution for types not trivially convertible to the type of the key
     * (here a std::unique_ptr), 'find' etc. member function calls will produce cryptic compiler diagnostics.
     */

    static_assert(TransparentlyHashable<Predicate>);
    std::unordered_set<std::unique_ptr<Predicate>, std::hash<Predicate>, std::equal_to<>> predicates;

    static_assert(TransparentlyHashable<ITerm>);
    std::unordered_set<std::unique_ptr<ITerm>, std::hash<ITerm>, std::equal_to<>> terms;
};

}

#endif
