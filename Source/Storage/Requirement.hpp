/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Requirement storage object
 * @author Oliver Dixon
 * @date 2025-04-26
 * @version Development
 */

#ifndef REQUIREMENT_HPP
#define REQUIREMENT_HPP

#include <gtkmm/treelistmodel.h>
#include <log4cxx/logger.h>

#include "../IR/MutableVariants/Sentences/IMutableSentence.hpp"
#include "../IR/Sentences/SentenceRoot.hpp"
#include "../IR/SymbolRepository.hpp"
#include "../UserTesting/Modelling/Test.hpp"
#include "../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "FOLLexer.hpp"
#include "StorageObjectBase.hpp"

namespace Gtk
{

class ListItem;
class Label;

} // namespace Gtk

namespace optifol
{

/**
 * @class Requirement
 * @brief The Requirement storage object is the atomic unit of measure in Optifol. It belongs to a single Subsystem.
 */
class Requirement : public StorageObjectBase,
                    public ITestModelNode
{
public:
    /**
     * @brief Create a new Requirement with the given name and register in the Glib GType system,
     *  but no active SymbolRepository.
     * @param name The initial name of the Requirement
     * @param statement The initial FOL statement of the Requirement
     * @param description The initial long-form description of the Requirement
     * @param priority The initial priority of the Requirement
     * @param tests The initial set of TestSpecificationEntry objects to template Test objects.
     * @param symbol_repository A null pointer to explicitly signify the lacking SymbolRepository
     * @warning As no system-wide symbol repository has been provided, this Requirement will not supply its symbols to
     *  the wider system. Logical analysis will produce unexpected results.
     */
    explicit Requirement(std::string&& name, std::string&& statement, std::string&& description, guint priority,
        Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>>&& tests, std::nullptr_t symbol_repository);

    /**
     * @brief Create a new Requirement with the given name and register in the Glib GType system
     * @param name The initial name of the Requirement
     * @param statement The initial FOL statement of the Requirement
     * @param description The initial long-form description of the Requirement
     * @param priority The initial priority of the Requirement
     * @param tests The initial set of TestSpecificationEntry objects to template Test objects.
     * @param symbol_repository The system-wide symbol repository with lifetimes guaranteed to cover that of the
     *  Requirement
     */
    explicit Requirement(std::string&& name, std::string&& statement, std::string&& description, guint priority,
        Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>>&& tests,
        std::shared_ptr<SymbolRepository> symbol_repository);

    /**
     * @brief Create a new Requirement with the given name and register in the Glib GType system
     * @param name The initial name of the Subsystem
     * @param statement The initial FOL statement of the Requirement
     * @param description The initial long-form description of the Requirement
     * @param priority The initial priority of the Requirement
     * @param tests The initial set of TestSpecificationEntry objects to template Test objects.
     * @param cobject The C cast-item used by Glib::Object
     * @param builder Currently unused builder parameter to provide to the Glib::Object instance
     * @param symbol_repository The system-wide symbol repository with lifetimes guaranteed to cover that of the
     *  Requirement
     */
    Requirement(std::string&& name, std::string&& statement, std::string&& description, guint priority,
        Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>>&& tests, BaseObjectType* cobject,
        const Glib::RefPtr<Gtk::Builder>& builder, std::shared_ptr<SymbolRepository> symbol_repository);

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_tests_tree() const noexcept override;

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_results_tree() const noexcept override;

    /**
     * @brief Compare two Requirement objects for semantic equality
     * @param other The Requirement with which to compare.
     * @return Is the current Requirement equivalent to the given other Requirement?
     */
    bool operator==(const Requirement &other) const noexcept;

    /**
     * @brief Get a read-write proxy for the 'statement' property
     * @return The read-write 'statement' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_statement();

    /**
     * @brief Get a read-write proxy for the 'description' property
     * @return The read-write 'description' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_description();

    /**
     * @brief Get a read-write proxy for the 'priority' property
     * @return The read-write 'priority' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<guint> property_priority();

    /**
     * @brief Get a read-write proxy for the 'normalised statement' property
     * @return The read-write 'normalised statement' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_normalised();

    [[nodiscard]] const Gio::ListStore<Test> * observe_tests() const noexcept;

    [[nodiscard]] Glib::RefPtr<Gio::ListStore<Test>> get_tests() const noexcept;

    [[nodiscard]] Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>> get_test_specs() const noexcept;

    /**
     * @brief Get a read-only proxy for the 'statement' property
     * @return The read-only 'statement' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_statement() const;

    /**
     * @brief Get a read-only proxy for the 'description' property
     * @return The read-only 'description' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_description() const;

    /**
     * @brief Get a read-only proxy for the 'priority' property
     * @return The read-only 'priority' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<guint> property_priority() const;

    /**
     * @brief Get a read-only proxy for the 'normalised statement' property
     * @return The read-only 'normalised statement' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_normalised() const;

    [[nodiscard]] std::string get_formatted_statement() const;

    [[nodiscard]] std::string_view observe_latex_statement() const noexcept;

    /**
     * @brief Determines the suitability of the Requirement for participation in formal analysis.
     * @return Does the Requirement have a prepared and normalised statement AST?
     */
    [[nodiscard]] bool is_analysis_ready() const noexcept;

    /**
     * @brief Determines whether the Requirement has at least one associated Test. This is commonly useful for
     *  determining its suitability to be added to a TestGroup.
     * @return Does the Requirement have at least one Test?
     */
    [[nodiscard]] bool has_tests() const noexcept;

    static void bind_name_to_label(const Glib::RefPtr<Gtk::ListItem> &item) noexcept;

private:
    static const log4cxx::LoggerPtr control_logger;
    static const log4cxx::LoggerPtr parse_logger;
    static const log4cxx::LoggerPtr cnf_logger;
    static const log4cxx::LoggerPtr integration_logger;

    /**
     * @brief Helper to push the given IMutableSentence node through a plain-text serialisation pipeline
     * @param sentence The sentence to serialise
     * @return The @ref std::string representation of the plain-text serialised sentence
     */
    static std::string text_serialise(const IMutableSentence *sentence);

    /**
     * @brief Helper to push the given IMutableSentence node through a LaTeX-text serialisation pipeline
     * @param sentence The sentence to serialise into math-mode LaTeX format
     * @return The @ref std:string representation of the LaTeX-escaped math-mode serialised sentence
     */
    static std::string latex_serialise(const IMutableSentence *sentence);

    /**
     * @brief Populate the core Requirement properties with the given initial values, and set up signals.
     * @param requirement_name The initial Requirement name
     * @param requirement_statement The initial Requirement statement text
     * @param requirement_description The initial Requirement description
     * @param requirement_priority The initial Requirement priority selection
     * @param requirement_tests The initial set of TestSpecificationEntry objects to template Test objects.
     */
    void setup_properties(std::string &&requirement_name, std::string &&requirement_statement,
            std::string &&requirement_description, guint requirement_priority,
            Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>> &&requirement_tests);

    /**
     * @brief Handle a change in the Requirement statement by re-parsing and updating internal state where necessary.
     */
    void handle_statement_change();

    void handle_test_spec_change(guint position, guint removed_count, guint added_count) const;

    Glib::Property<Glib::ustring> statement;
    Glib::Property<Glib::ustring> normalised_statement;
    Glib::Property<Glib::ustring> description;
    Glib::Property<guint> priority;

    Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>> test_specs = Gio::ListStore<TestSpecificationEntry>::create();
    const Glib::RefPtr<Gio::ListStore<Test>> tests = Gio::ListStore<Test>::create();
    const Glib::RefPtr<Gtk::TreeListModel> tests_tree =
            Gtk::TreeListModel::create(tests, &ITestModelNode::get_given_tests_tree, true);

    std::unique_ptr<MutableSentenceRoot> original_ast;
    std::unique_ptr<SentenceRoot> prepared_ast;
    std::shared_ptr<SymbolRepository> symbol_repository;

    std::string formatted_input_statement;
    std::string latex_input_statement;

    static std::istringstream lexer_input_stream;
    static FOLLexer lexer;
    static FOLParser parser;
};

} // namespace optifol

#endif
