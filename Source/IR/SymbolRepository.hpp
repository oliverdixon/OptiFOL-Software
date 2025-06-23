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
    template<typename TermType = IProcessedTerm> requires std::derived_from<TermType, IProcessedTerm>
    const TermType *add_symbol(std::unique_ptr<TermType> &&term)
    {
        const TermType * given_handle = term.get();
        auto [inserted_it, inserted] = terms.insert(std::move(term));

        if (!inserted) {
            const auto existing_it = terms.find(*given_handle);
            const TermType * downcast_ptr = nullptr;

            if (existing_it == terms.cend() || (downcast_ptr =
                    dynamic_cast<const TermType *>(existing_it->get())) == nullptr)
                throw std::runtime_error("Term " + std::string(given_handle->get_disambiguated_name()) +
                                         " could not be inserted or located in the symbol repository.");

            return downcast_ptr;
        }

        return given_handle;
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
    template<typename SentenceType = ISentence> requires std::derived_from<SentenceType, ISentence>
    const SentenceType *add_symbol(std::unique_ptr<SentenceType> &&sentence)
    {
        const SentenceType * given_handle = sentence.get();
        auto [inserted_it, inserted] = sentences.insert(std::move(sentence));

        if (!inserted) {
            const auto existing_it = sentences.find(*given_handle);
            const SentenceType * downcast_ptr = nullptr;

            if (existing_it == sentences.cend() || (downcast_ptr =
                    dynamic_cast<const SentenceType *>(existing_it->get())) == nullptr)
                throw std::runtime_error("Sentence could not be inserted or located in the symbol repository.");

            return downcast_ptr;
        }

        return given_handle;
    }

    /**
     * @brief Retrieves a handle to an immutable term symbol owned by the repository
     * @param term A hash-equal ITerm to the target term
     * @return A constant handle to the term, if a suitable match exists in the repository. Otherwise, an empty \ref
     *  std::optional.
     */
    std::optional<const IProcessedTerm *> get_symbol_handle(const IProcessedTerm &term) const;

    /**
     * @brief Retrieves a handle to an immutable sentence symbol owned by the repository
     * @param sentence A hash-equal sentence to the target sentence
     * @return A constant handle to the sentence, if a suitable match exists in the repository. Otherwise, an empty
     * \ref std::optional.
     */
    std::optional<const ISentence *> get_symbol_handle(const ISentence &sentence) const;

    /*
     * Note that we verify the satisfaction of sentence and IProcessedTerms against the TransparentlyHashable concept
     * here in the class definition to produce readable error messages. If the hasher and equality functor call
     * operators cannot participate in overload resolution for types not trivially convertible to the type of the key
     * (here a std::unique_ptr), 'find' etc. member function calls will produce cryptic compiler diagnostics.
     */

    static_assert(TransparentlyHashable<ISentence>);
    std::unordered_set<std::unique_ptr<ISentence>, std::hash<ISentence>, std::equal_to<>> sentences;

    static_assert(TransparentlyHashable<IProcessedTerm>);
    std::unordered_set<std::unique_ptr<IProcessedTerm>, std::hash<IProcessedTerm>, std::equal_to<>> terms;
};

} // namespace optifol

#endif
