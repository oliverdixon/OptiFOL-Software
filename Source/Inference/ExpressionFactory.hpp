/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the CNF expression factory
 * @author Oliver Dixon
 * @date 2025-09-14
 * @version Development
 */

#ifndef OPTIFOL_EXPRESSIONFACTORY_HPP
#define OPTIFOL_EXPRESSIONFACTORY_HPP

#include <log4cxx/logger.h>

#include "../Visitors/MutableTargets/Observers/TextSerialiserVisitor.hpp"

namespace optifol
{

class MutableSentenceRoot;
class SentenceRoot;
class SymbolRepository;

/**
 * @class ExpressionFactory
 * @brief The ExpressionFactory provides a single-method static factory for normalising FOL sentences produced
 * by the FOLParser and registering the symbols in a shared-ownership SymbolRepository.
 */
class ExpressionFactory
{
public:
    /**
     * @brief Produce an immutable, CNF-normalised SentenceRoot from the given de-normalised
     * MutableSentenceRoot.
     * @param sentence_root The de-normalised MutableSentenceRoot, typically provided by a FOLParser.
     * @param symbol_repository The shared-ownership SymbolRepository into which expression symbols should be
     *  registered.
     * @return The normalised SentenceRoot.
     * @throws SemanticException A semantic/logical error was encountered with the expression during
     * normalisation or standardisation.
     */
    static std::unique_ptr<SentenceRoot> build_sentence(std::unique_ptr<MutableSentenceRoot> &&sentence_root,
            std::shared_ptr<SymbolRepository> symbol_repository);

private:
    /**
     * @brief Push an arbitrary-form MutableSentenceRoot through the seven-stage CNF normalisation pipeline,
     * but do not transform into the immutable representation or register any symbols in a SymbolRepository.
     * @param denormalised_root The de-normalised sentence root to normalise into CNF.
     * @return The MutableSentenceRoot of the CNF-normalised tree.
     * @throws SemanticException A semantic/logical error was encountered with the expression during
     * normalisation.
     */
    static std::unique_ptr<MutableSentenceRoot> cnf_normalise(
            std::unique_ptr<MutableSentenceRoot> &&denormalised_root);

    /**
     * @brief Build an immutable SentenceRoot from the given CNF-normalised MutableSentenceRoot, and register
     * symbols in the shared-ownership SymbolRepository.
     * @param normalised_root The MutableSentenceRoot of the CNF sentence.
     * @param symbol_repository The shared-ownership SymbolRepository into which expression symbols should be
     *  registered.
     * @return The normalised SentenceRoot.
     * @throws SemanticException A semantic/logical error was encountered with the expression during
     * standardisation.
     */
    static std::unique_ptr<SentenceRoot> build_symbol_repository(
            std::unique_ptr<MutableSentenceRoot> &&normalised_root,
            std::shared_ptr<SymbolRepository> symbol_repository);

    static const log4cxx::LoggerPtr cnf_logger;
    static TextSerialiserVisitor serialiser_visitor;
};

} // namespace optifol

#endif // OPTIFOL_EXPRESSIONFACTORY_HPP
