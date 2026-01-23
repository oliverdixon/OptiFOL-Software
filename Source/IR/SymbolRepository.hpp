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
#include "../Optifol.hpp"
#include "Sentences/IProcessedSentence.hpp"
#include "Terms/IProcessedTerm.hpp"
#include "Terms/Variable.hpp"

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
            throw std::runtime_error("Cannot add term: insertion failed.");

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
    template<typename SentenceType = IProcessedSentence>
        requires std::derived_from<SentenceType, IProcessedSentence>
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
    const IProcessedTerm *get_symbol_handle(const IProcessedTerm &term) const;

    /**
     * @brief Retrieves a handle to an immutable sentence symbol owned by the repository
     * @tparam SentenceType The type of sentence pointer to return
     * @param sentence A hash-equal sentence to the target sentence
     * @return A constant handle to the sentence, if a suitable match exists in the repository. Otherwise, an empty
     *  @ref std::optional.
     */
    template<typename SentenceType = IProcessedSentence>
        requires std::derived_from<SentenceType, IProcessedSentence>
    const SentenceType *get_symbol_handle(const IProcessedSentence &sentence) const
    {
        const auto it = sentences.find(sentence);
        const SentenceType * downcast_ptr = nullptr;

        if (it == sentences.cend() || (downcast_ptr = dynamic_cast<const SentenceType *>(it->get())) == nullptr)
            return nullptr;

        return downcast_ptr;
    }

    const Variable *get_symbol_handle(const Variable &variable) const;

    /**
     * @brief Determine equality between two SymbolRepository objects according to their stored sentences and terms.
     * @param other The SymbolRepository with which to compare elements.
     * @return Do the SymbolRepository objects store the same items?
     */
    [[nodiscard]] bool operator==(const SymbolRepository & other) const noexcept;

    /**
     * @brief Move all symbols (sentences, non-variable and variable terms) from the given SymbolRepository into this
     *  one.
     * @param other The target repository, the entirety of which is to be consumed by the inheritor.
     */
    void inherit_repository(std::unique_ptr<SymbolRepository> &&other);

private:
    /**
     * @brief Steal @ref std::unique_ptr objects from the given UniqueUnorderedSet source container and move into the
     *  given destination.
     * @tparam Element The type of elements owned by the members of the set.
     * @param dest The destination set.
     * @param src The source set.
     */
    template<class Element>
    static void inherit_set(UniqueUnorderedSet<Element>& dest, UniqueUnorderedSet<Element>& src)
    {
        for (auto node_handle_it = src.begin(); node_handle_it != src.end(); ) {
            auto borrowed_node = src.extract(node_handle_it++);
            dest.insert(std::move(borrowed_node));
        }
    }

    UniqueUnorderedSet<IProcessedSentence> sentences;
    UniqueUnorderedSet<IProcessedTerm> terms;
    UniqueUnorderedSet<Variable> variables;
};

template<>
inline const Variable* SymbolRepository::add_symbol(std::unique_ptr<Variable>&& term)
{
    const auto find_it = variables.find(*term);
    if (find_it != variables.cend())
        return find_it->get();

    const auto [inserted_it, success] = variables.insert(std::move(term));
    if (!success)
        throw std::runtime_error("Cannot add variable: insertion failed.");

    return inserted_it->get();
}

} // namespace optifol

#endif
