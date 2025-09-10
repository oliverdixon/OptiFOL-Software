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

#include "../HashableEqualityFunctor.hpp"
#include "../IHashable.hpp"
#include "Sentences/ISentence.hpp"
#include "Terms/IProcessedTerm.hpp"

namespace optifol
{

/**
 * @class SymbolRepository
 * @brief A SymbolRepository persists a single set of sentenceNodes and ITerms with guaranteed lifetimes. The
 *  repository can be populated and queried, exposing constant pointers, but elements cannot be removed or destructed
 *  until the entire repository is destructed.
 */
class SymbolRepository
{
public:
    /**
     * @brief Add a generic term to the repository
     * @tparam TermType The type of term pointer to return
     * @param term A \ref std::unique_ptr wrapper of the ITerm whose ownership should be transferred.
     * @return A constant handle to the term in the repository
     * @throws std::runtime_error An equivalent term does not exist in the repository, and could not be added.
     * @note If the supplied term is hash-equal to an existing term held by the repository, the repository is unchanged.
     */
    template<typename TermType = IProcessedTerm>
        requires std::derived_from<TermType, IProcessedTerm>
    const TermType *add_symbol(std::unique_ptr<TermType> &&term)
    {
        const auto find_it = terms.find(*term);
        if (find_it != terms.cend()) {
            // If a hash-equal element is already in the set, attempt to downcast to the requested type and return.
            const auto downcast_ptr = dynamic_cast<const TermType *>(find_it->get());
            if (downcast_ptr == nullptr)
                throw std::runtime_error("Cannot add term " + std::string(term->get_disambiguated_name()) +
                        ": a matching term of a different type already exists in the repository.");
            return downcast_ptr;
        }

        // Otherwise, add the new element by transferring ownership to the set.
        const auto [inserted_it, success] = terms.insert(std::move(term));
        if (!success)
            throw std::runtime_error(
                    "Cannot add term " + std::string(term->get_disambiguated_name()) + ": insertion failed.");

        /*
         * This is a bit dodgy, as the compiler isn't enforcing semantics correctness of the pointer cast, as would be
         * suggested by use of static_cast. It could equally be cast to any other pointer and be undefined at
         * dereference. But the STL standard provides this guarantee, as if the insertion is successful, the returned
         * iterator is defined to detain a type of the key, i.e. std::unique_ptr<IProcessedTerm>. We know from the
         * function signature that our term holds a pointer of type TermType.
         */
        return static_cast<const TermType *>(inserted_it->get());
    }

    /**
     * @brief Add a sentence to the repository
     * @tparam SentenceType The type of sentence pointer to return
     * @param sentence A \ref std::unique_ptr wrapper of the sentence whose ownership should be transferred.
     * @return A constant handle to the sentence in the repository
     * @throws std::runtime_error An equivalent sentence does not exist in the repository, and could not be added.
     * @note If the supplied sentence is hash-equal to an existing sentence held by the repository, the repository is
     *  unchanged.
     */
    template<typename SentenceType = ISentence>
        requires std::derived_from<SentenceType, ISentence>
    const SentenceType *add_symbol(std::unique_ptr<SentenceType> &&sentence)
    {
        const auto find_it = sentences.find(*sentence);
        if (find_it != sentences.cend()) {
            const auto downcast_ptr = dynamic_cast<const SentenceType *>(find_it->get());
            if (downcast_ptr == nullptr)
                throw std::runtime_error("Cannot add sentence: a matching sentence of a different type already exists "
                                         "in the repository.");
            return downcast_ptr;
        }

        const auto [inserted_it, success] = sentences.insert(std::move(sentence));
        if (!success)
            throw std::runtime_error("Cannot add sentence: insertion failed.");

        return static_cast<const SentenceType *>(inserted_it->get());
    }

    /**
     * @brief Retrieves a handle to an immutable term symbol owned by the repository
     * @param term A hash-equal ITerm to the target term
     * @return A constant handle to the term, if a suitable match exists in the repository. Otherwise, an empty @ref
     *  std::optional.
     */
    std::optional<const IProcessedTerm *> get_symbol_handle(const IProcessedTerm &term) const;

    /**
     * @brief Retrieves a handle to an immutable sentence symbol owned by the repository
     * @param sentence A hash-equal sentence to the target sentence
     * @return A constant handle to the sentence, if a suitable match exists in the repository. Otherwise, an empty
     *  @ref std::optional.
     */
    std::optional<const ISentence *> get_symbol_handle(const ISentence &sentence) const;

private:
    std::unordered_set<std::unique_ptr<ISentence>, std::hash<ISentence>, HashableEqualityFunctor<ISentence>> sentences;
    std::unordered_set<std::unique_ptr<IProcessedTerm>, std::hash<IProcessedTerm>,
            HashableEqualityFunctor<IProcessedTerm>>
            terms;
};

} // namespace optifol

#endif
