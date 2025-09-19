/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the CNF expression factory
 * @author Oliver Dixon
 * @date 2025-09-14
 * @version Development
 */

#include "ExpressionFactory.hpp"

#include "../Exceptions/SemanticException.hpp"
#include "../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../IR/Sentences/SentenceRoot.hpp"
#include "../Logging.hpp"

#include "../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/DMLVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/QuantifierExtractingVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/SkolemIntroducingVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/SymbolStandardisingVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/UniversalEliminationVisitor.hpp"

namespace optifol
{

const log4cxx::LoggerPtr ExpressionFactory::cnf_logger = Logging::get_logger({"LogicServices", "CNFNormalisation"});
TextSerialiserVisitor ExpressionFactory::serialiser_visitor;

std::unique_ptr<SentenceRoot> ExpressionFactory::build_sentence(
        std::unique_ptr<MutableSentenceRoot> &&sentence_root, std::shared_ptr<SymbolRepository> symbol_repository)
{
    /*
     * Step 1. Push through the seven-stage CNF normalisation pipeline.
     *
     * This produces a MutableSentenceRoot that is structured as a tree, but only contains elements allowable in a CNF
     * tree. Note that a new tree is not created; the original tree is mutated such that it can be trivially converted
     * to the conjunctive-disjunctive set form.
     *
     * If the CNF logger is configured to an info level, the original ("before") and normalised ("after") CNF statements
     * are serialised with the TextSerialiserVisitor.
     */

    if (cnf_logger->isInfoEnabled()) {

        cnf_logger->info("Beginning CNF pipeline transformation.");
        sentence_root->accept(serialiser_visitor);
        cnf_logger->info("Initial sentence: " + serialiser_visitor.extract());

        sentence_root = cnf_normalise(std::move(sentence_root));

        cnf_logger->info("Completed CNF transformation.");
        sentence_root->accept(serialiser_visitor);
        cnf_logger->info("Normalised sentence: " + serialiser_visitor.extract());
    } else
        sentence_root = cnf_normalise(std::move(sentence_root));

    /*
     * Step 2. Populate the symbol repository.
     *
     * This transforms the normalised mutable CNF tree into the corresponding immutable form, represented by a
     * SentenceRoot. SentenceRoot objects do not indicate trees, rather sets of literals under disjunction, of which the
     * elements are under conjunction. The given SymbolRepository is also populated with the terms and literals
     * appearing in the normalised expression.
     */

    return build_symbol_repository(std::move(sentence_root), std::move(symbol_repository));
}

std::unique_ptr<MutableSentenceRoot> ExpressionFactory::cnf_normalise(
        std::unique_ptr<MutableSentenceRoot> &&denormalised_root)
{
    // clang-format off
    const std::array<std::unique_ptr<MutatingSentenceVisitorBase>, 7> cnf_visitors{
        std::make_unique<ImplicationEliminationVisitor>(),
        std::make_unique<DMLVisitor>(),
        std::make_unique<SymbolStandardisingVisitor>(),
        std::make_unique<QuantifierExtractingVisitor>(),
        std::make_unique<SkolemIntroducingVisitor>(),
        std::make_unique<UniversalEliminationVisitor>(),
        std::make_unique<DisjunctionDistributionVisitor>()
    };
    // clang-format on

    if (cnf_logger->isDebugEnabled())
        /*
         * Explicitly check if debugging is enabled on the CNF logger, as running a serialisation visitor down the
         * entire tree for each step in the normalisation pipeline would be a great inefficiency if the strings were not
         * used!
         */
        for (const auto &visitor: cnf_visitors) {
            try {
                denormalised_root->accept(*visitor);
            } catch (const SemanticException &semantic_exception) {
                Logging::get_logger({cnf_logger->getName(), std::string(visitor->get_visitor_name())})
                        ->error(semantic_exception.what());
                throw;
            }

            // Serialise result of normalising stage.
            denormalised_root->accept(serialiser_visitor);
            Logging::get_logger({cnf_logger->getName(), std::string(visitor->get_visitor_name())})
                    ->debug(serialiser_visitor.extract());
        }
    else
        for (const auto &visitor: cnf_visitors)
            try {
                denormalised_root->accept(*visitor);
            } catch (const SemanticException &semantic_exception) {
                Logging::get_logger({cnf_logger->getName(), std::string(visitor->get_visitor_name())})
                        ->error(semantic_exception.what());
                throw;
            }

    return denormalised_root;
}

std::unique_ptr<SentenceRoot> ExpressionFactory::build_symbol_repository(
        std::unique_ptr<MutableSentenceRoot> &&normalised_root, std::shared_ptr<SymbolRepository> symbol_repository)
{
    RepositoryBuildingVisitor building_visitor(std::move(symbol_repository));
    normalised_root->accept(building_visitor);
    return building_visitor.take_last_root();
}

} // namespace optifol
