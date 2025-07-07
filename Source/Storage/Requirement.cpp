/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the requirement-level storage object
 * @author Oliver Dixon
 * @date 2025-04-26
 * @version Development
 */

#include "Requirement.hpp"

#include <cassert>

#include "../Exceptions/SemanticException.hpp"
#include "../Logging.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/DMLVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/QuantifierExtractingVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/SkolemIntroducingVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/SymbolStandardisingVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/UniversalEliminationVisitor.hpp"
#include "../Visitors/MutableTargets/Observers/LaTeXSerialisationVisitor.hpp"
#include "../Visitors/MutableTargets/Observers/TextSerialiserVisitor.hpp"

namespace optifol
{

std::istringstream Requirement::lexer_input_stream;

/*
 * TODO: this construction is possibly undefined due to std::cerr. But for the real case, we'll use a custom error
 *  handler that can be statically initialised in the Analysis Manager, so this is OK for development.
 */
FOLLexer Requirement::lexer{Requirement::lexer_input_stream, std::cerr};
FOLParser Requirement::parser{&Requirement::lexer};

log4cxx::LoggerPtr Requirement::cnf_logger = Logging::get_logger({"LogicServices", "CNFNormalisation"});
log4cxx::LoggerPtr Requirement::parse_logger = Logging::get_logger({"LogicServices", "FormalParsing"});
log4cxx::LoggerPtr Requirement::integration_logger = Logging::get_logger({"LogicServices", "SystemIntegration"});

Requirement::Requirement(
        std::string &&name, std::string &&statement, std::string &&description, guint priority, std::nullptr_t) :
    Glib::ObjectBase("Requirement"),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority")
{
    setup_properties(std::move(name), std::move(statement), std::move(description), priority);
}

Requirement::Requirement(std::string &&name, std::string &&statement, std::string &&description, const guint priority,
        SymbolRepository &system_repository) :
    Glib::ObjectBase("Requirement"),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority"),
    repository_building_visitor(system_repository)
{
    setup_properties(std::move(name), std::move(statement), std::move(description), priority);
}

Requirement::Requirement(std::string &&name, std::string &&statement, std::string &&description, const guint priority,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder, SymbolRepository &system_repository) :
    Glib::ObjectBase("Requirement"),
    StorageObjectBase(cobject, builder),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority"),
    repository_building_visitor(system_repository)
{
    setup_properties(std::move(name), std::move(statement), std::move(description), priority);
}

Glib::PropertyProxy<Glib::ustring> Requirement::property_statement()
{
    return statement.get_proxy();
}

Glib::PropertyProxy<Glib::ustring> Requirement::property_description()
{
    return description.get_proxy();
}

Glib::PropertyProxy<guint> Requirement::property_priority()
{
    return priority.get_proxy();
}

Glib::PropertyProxy<Glib::ustring> Requirement::property_normalised()
{
    return normalised_statement.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Requirement::property_statement() const
{
    return statement.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Requirement::property_description() const
{
    return description.get_proxy();
}

Glib::PropertyProxy_ReadOnly<guint> Requirement::property_priority() const
{
    return priority.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Requirement::property_normalised() const
{
    return normalised_statement.get_proxy();
}

std::string Requirement::get_formatted_statement() const
{
    return formatted_input_statement;
}

std::string_view Requirement::observe_latex_statement() const
{
    return latex_input_statement;
}

void Requirement::setup_properties(std::string &&requirement_name, std::string &&requirement_statement,
        std::string &&requirement_description, const guint requirement_priority)
{
    property_statement().signal_changed().connect(
            [this]
            {
                // If the statement has changed, pass it through the parser and normaliser.
                if (!property_statement().get_value().empty()) {
                    lexer_input_stream.str(property_statement().get_value());

                    try {
                        parser.parse();
                    } catch (const ParseError &parse_error) {
                        parse_logger->error(parse_error.what());
                        return;
                    }

                    original_ast = parser.retrieve_sentence();
                    formatted_input_statement = text_serialise(original_ast.get());
                    latex_input_statement = latex_serialise(original_ast.get());

                    try {
                        // Perform CNF normalisation followed by population of the symbol repository
                        prepared_ast = populate_symbol_repository(cnf_normalise(original_ast->clone()));
                    } catch (const SemanticException &) {
                        cnf_logger->error("Preparation process was unsuccessful due to invalid logical semantics; "
                                         "requirements will be missing.");
                        return;
                    }

                    std::ostringstream serialiser_stream;
                    prepared_ast->serialise(serialiser_stream);
                    property_normalised().set_value(serialiser_stream.str());
                }
            });

    property_name().set_value(std::move(requirement_name));
    property_statement().set_value(std::move(requirement_statement));
    property_description().set_value(std::move(requirement_description));
    property_priority().set_value(requirement_priority);
}

std::unique_ptr<IMutableSentence> Requirement::cnf_normalise(std::unique_ptr<IMutableSentence> &&sentence)
{
    if (cnf_logger->isInfoEnabled()) {
        cnf_logger->info("Beginning CNF pipeline transformation.");
        cnf_logger->info("Initial sentence: " + text_serialise(sentence.get()));
    }

    const std::array<std::unique_ptr<MutatingSentenceVisitorBase>, 7> visitors{
            std::make_unique<ImplicationEliminationVisitor>(), std::make_unique<DMLVisitor>(),
            std::make_unique<SymbolStandardisingVisitor>(), std::make_unique<QuantifierExtractingVisitor>(),
            std::make_unique<SkolemIntroducingVisitor>(), std::make_unique<UniversalEliminationVisitor>(),
            std::make_unique<DisjunctionDistributionVisitor>()};

    if (cnf_logger->isDebugEnabled())
        /*
         * Explicitly check if debugging is enabled on the CNF logger, as running a serialisation visitor down the
         * entire tree for each step in the normalisation pipeline would be a great inefficiency if the strings were not
         * used!
         */
        for (const auto &visitor: visitors) {
            try {
                sentence->accept(*visitor);
            } catch (const SemanticException &semantic_exception) {
                Logging::get_logger({cnf_logger->getName(), std::string(visitor->get_visitor_name())})
                        ->error(semantic_exception.what());
                throw;
            }

            Logging::get_logger({cnf_logger->getName(), std::string(visitor->get_visitor_name())})
                    ->debug(text_serialise(sentence.get()));
        }
    else
        for (const auto &visitor: visitors)
            try {
                sentence->accept(*visitor);
            } catch (const SemanticException &semantic_exception) {
                Logging::get_logger({cnf_logger->getName(), std::string(visitor->get_visitor_name())})
                        ->error(semantic_exception.what());
                throw;
            }

    if (cnf_logger->isInfoEnabled()) {
        cnf_logger->info("Completed CNF transformation.");
        cnf_logger->info("Normalised sentence: " + text_serialise(sentence.get()));
    }

    return sentence;
}

std::unique_ptr<SentenceRoot> Requirement::populate_symbol_repository(std::unique_ptr<IMutableSentence> &&mutable_root)
{
    if (repository_building_visitor.has_value() == false)
        throw std::logic_error("Requirement has not been exposed to the system-wide symbol repository; formula "
                               "cannot be understood within the context of adjacent expressions. Logical analysis "
                               "will produce unexpected results.");

    // TODO: info-level integration logging as with CNF normalisation pipeline

    std::ignore = mutable_root->accept(*repository_building_visitor);
    return repository_building_visitor->take_last_root();
}

std::string Requirement::text_serialise(const IMutableSentence *sentence)
{
    TextSerialiserVisitor text_serialiser_visitor;
    sentence->accept(text_serialiser_visitor);
    return text_serialiser_visitor.extract();
}

std::string Requirement::latex_serialise(const IMutableSentence *sentence)
{
    LaTeXSerialisationVisitor latex_serialisation_visitor;
    sentence->accept(latex_serialisation_visitor);
    return latex_serialisation_visitor.extract();
}

} // namespace optifol
