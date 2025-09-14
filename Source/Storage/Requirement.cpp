/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Requirement storage object
 * @author Oliver Dixon
 * @date 2025-04-26
 * @version Development
 */

#include "Requirement.hpp"

#include <gtkmm/label.h>
#include <gtkmm/listitem.h>

#include "../Exceptions/SemanticException.hpp"
#include "../Inference/ExpressionFactory.hpp"
#include "../Logging.hpp"
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

const log4cxx::LoggerPtr Requirement::control_logger = Logging::get_logger({"GUI", "StorageControl", "Requirement"});

const log4cxx::LoggerPtr Requirement::cnf_logger = Logging::get_logger({"LogicServices", "CNFNormalisation"});
const log4cxx::LoggerPtr Requirement::parse_logger = Logging::get_logger({"LogicServices", "FormalParsing"});
const log4cxx::LoggerPtr Requirement::integration_logger = Logging::get_logger({"LogicServices", "SystemIntegration"});

Requirement::Requirement(std::string &&name, std::string &&statement, std::string &&description, const guint priority,
        Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>>&& tests, std::nullptr_t) :
    Glib::ObjectBase("Requirement"),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority")
{
    setup_properties(std::move(name), std::move(statement), std::move(description), priority, std::move(tests));
}

Requirement::Requirement(std::string &&name, std::string &&statement, std::string &&description, const guint priority,
        Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>>&& tests,
        std::shared_ptr<SymbolRepository> symbol_repository) :
    Glib::ObjectBase("Requirement"),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority"),
    symbol_repository(std::move(symbol_repository))
{
    setup_properties(std::move(name), std::move(statement), std::move(description), priority, std::move(tests));
}

Requirement::Requirement(std::string &&name, std::string &&statement, std::string &&description, const guint priority,
        Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>>&& tests, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder, std::shared_ptr<SymbolRepository> symbol_repository) :
    Glib::ObjectBase("Requirement"),
    StorageObjectBase(cobject, builder),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority"),
    symbol_repository(std::move(symbol_repository))
{
    setup_properties(std::move(name), std::move(statement), std::move(description), priority, std::move(tests));
}

Glib::RefPtr<Gtk::TreeListModel> Requirement::get_tests_tree() const noexcept
{
    return tests_tree;
}

Glib::RefPtr<Gtk::TreeListModel> Requirement::get_results_tree() const noexcept
{
    return tests_tree;
}

bool Requirement::operator==(const Requirement & other) const noexcept
{
    return hash() == other.hash();
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

const Gio::ListStore<Test> *Requirement::observe_tests() const noexcept
{
    return tests.get();
}

Glib::RefPtr<Gio::ListStore<Test>> Requirement::get_tests() const noexcept
{
    return tests;
}

Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>> Requirement::get_test_specs() const noexcept
{
    return test_specs;
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

std::string_view Requirement::observe_latex_statement() const noexcept
{
    return latex_input_statement;
}

bool Requirement::is_analysis_ready() const noexcept
{
    return prepared_ast != nullptr;
}

bool Requirement::has_tests() const noexcept
{
    return tests->get_n_items() > 0;
}

void Requirement::bind_name_to_label(const Glib::RefPtr<Gtk::ListItem> &item) noexcept
{
    const auto target_label = dynamic_cast<Gtk::Label *>(item->get_child());
    const auto typed_group = dynamic_cast<const Requirement *>(item->get_item().get());
    if (target_label == nullptr || typed_group == nullptr)
        return;

    target_label->set_text(typed_group->property_name().get_value());
}

void Requirement::setup_properties(std::string &&requirement_name, std::string &&requirement_statement,
        std::string &&requirement_description, const guint requirement_priority,
        Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>> &&requirement_tests)
{
    property_statement().signal_changed().connect(sigc::mem_fun(*this, &Requirement::handle_statement_change));

    property_name().set_value(std::move(requirement_name));
    property_statement().set_value(std::move(requirement_statement));
    property_description().set_value(std::move(requirement_description));
    property_priority().set_value(requirement_priority);
    test_specs = std::move(requirement_tests);

    test_specs->signal_items_changed().connect(sigc::mem_fun(*this, &Requirement::handle_test_spec_change));
    handle_test_spec_change(0, 0, test_specs->get_n_items()); // On first setup, all items are new.
}

void Requirement::handle_statement_change()
{
    // If the statement has changed, pass it through the parser and normaliser.
    const auto &typed_statement = property_statement().get_value();

    if (typed_statement.empty() == true) {
        // TODO should compartmentalise all statement-related structures into a struct
        original_ast.reset();
        formatted_input_statement.clear();
        latex_input_statement.clear();
        prepared_ast.reset();

        control_logger->debug("Removed FOL statement from requirement \"" + property_name().get_value() + "\".");
        return;
    }

    lexer_input_stream.str(typed_statement);

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
        prepared_ast = ExpressionFactory::build_sentence(original_ast->clone_as_root(), symbol_repository);
    } catch (const SemanticException &) {
        cnf_logger->error("Preparation process was unsuccessful due to invalid logical semantics; "
                          "requirements will be missing.");
        return;
    }

    std::ostringstream serialiser_stream;
    prepared_ast->serialise(serialiser_stream);
    property_normalised().set_value(serialiser_stream.str());

    control_logger->debug("Successfully updated FOL statement for requirement \"" +
        property_name().get_value() + "\".");
}

void Requirement::handle_test_spec_change(const guint position, const guint removed_count, const guint added_count)
    const
{
    std::vector<Glib::RefPtr<Test>> new_tests;
    new_tests.reserve(added_count);

    for (guint spec_index = position; spec_index < added_count; ++spec_index) {
        const auto& spec = test_specs->get_item(spec_index);
        if (spec == nullptr || spec->property_executable().get_value() == nullptr)
            control_logger->warn("Ignoring invalid test specification at index " + std::to_string(spec_index));
        else
            new_tests.push_back(Glib::make_refptr_for_instance(new Test(spec)));
    }

    tests->splice(position, removed_count, new_tests);
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
