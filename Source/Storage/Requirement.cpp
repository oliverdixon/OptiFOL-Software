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

#include "../Visitors/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/DMLVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/QuantifierExtractingVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/SkolemIntroducingVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/SymbolStandardisingVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/UniversalEliminationVisitor.hpp"
#include "../Visitors/Sentences/Serialisers/TextSerialiserVisitor.hpp"

namespace optifol
{

std::istringstream Requirement::lexer_input_stream;

/*
 * TODO: this construction is possibly undefined due to std::cerr. But for the real case, we'll use a custom error
 *  handler that can be statically initialised in the Analysis Manager, so this is OK for development.
 */
FOLLexer Requirement::lexer{Requirement::lexer_input_stream, std::cerr};
FOLParser Requirement::parser{&Requirement::lexer};


Requirement::Requirement(std::string&& name, std::string&& statement, std::string&& description, const guint priority) :
    Glib::ObjectBase("Requirement"),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority")
{
    setup_properties(std::move(name), std::move(statement), std::move(description), priority);
}

Requirement::Requirement(std::string&& name, std::string&& statement, std::string&& description, const guint priority,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Requirement"),
    StorageObjectBase(cobject, builder),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority")
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

void Requirement::setup_properties(std::string&& name, std::string&& statement, std::string&& description,
                                   const guint priority)
{
    property_statement().signal_changed().connect([this]
    {
        // If the statement has changed, pass it through the parser and normaliser. TODO: error-checking.
        if (!property_statement().get_value().empty()) {
            lexer_input_stream.str(property_statement().get_value());
            parser.parse();
            original_ast = parser.retrieve_sentence();
            formatted_input_statement = text_serialise(original_ast.get());

            cnf_renormalise();
            property_normalised().set_value(text_serialise(cnf_ast.get()));
        }
    });

    property_name().set_value(std::move(name));
    property_statement().set_value(std::move(statement));
    property_description().set_value(std::move(description));
    property_priority().set_value(priority);
}

void Requirement::cnf_renormalise()
{
    assert(original_ast != nullptr);

    auto borrowed_sentence = std::move(original_ast);

    // Step 1: Implication Elimination
    auto implication_elimination_visitor = ImplicationEliminationVisitor();
    borrowed_sentence->accept(implication_elimination_visitor);

    // Step 2: De Morgan's Law
    auto demorgan_visitor = DMLVisitor();
    borrowed_sentence->accept(demorgan_visitor);

    // Step 3: Variable standardisation
    auto variable_standardising_visitor = SymbolStandardisingVisitor();
    borrowed_sentence->accept(variable_standardising_visitor);

    // Step 4: Quantifier extraction
    auto quantifier_extraction_visitor = QuantifierExtractingVisitor();
    borrowed_sentence->accept(quantifier_extraction_visitor);

    // Step 5: Skolem function introduction
    auto skolem_introducing_visitor = SkolemIntroducingVisitor();
    borrowed_sentence->accept(skolem_introducing_visitor);

    // Step 6: Universal elimination
    auto universal_elimination_visitor = UniversalEliminationVisitor();
    borrowed_sentence->accept(universal_elimination_visitor);

    // Step 7: Disjunction distribution
    auto disjunction_distribution_visitor = DisjunctionDistributionVisitor();
    borrowed_sentence->accept(disjunction_distribution_visitor);

    cnf_ast = std::move(borrowed_sentence);
}

std::string Requirement::text_serialise(const ISentenceNode *sentence)
{
    static TextSerialiserVisitor text_serialiser_visitor;
    sentence->accept(text_serialiser_visitor);
    return text_serialiser_visitor.extract();
}

}
