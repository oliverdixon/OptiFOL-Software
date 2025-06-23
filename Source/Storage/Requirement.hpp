/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the requirement-level storage object
 * @author Oliver Dixon
 * @date 2025-04-26
 * @version Development
 */

#ifndef REQUIREMENT_HPP
#define REQUIREMENT_HPP

#include <log4cxx/logger.h>

#include "../IR/MutableVariants/Sentences/IMutableSentence.hpp"
#include "../IR/SymbolRepository.hpp"
#include "../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../IR/Sentences/SentenceRoot.hpp"
#include "FOLLexer.hpp"
#include "StorageObjectBase.hpp"

namespace optifol
{

/**
 * @class Requirement
 * @brief The Requirement storage object is the atomic unit of measure in Optifol. It belongs to a single Subsystem.
 */
class Requirement :
        public StorageObjectBase
{
public:
    /**
     * @brief Create a new Requirement with the given name and register in the Glib GType system,
     *  but no active SymbolRepository.
     * @param name The initial name of the Requirement
     * @param statement The initial FOL statement of the Requirement
     * @param description The initial long-form description of the Requirement
     * @param priority The initial priority of the Requirement
     * @param system_repository A null pointer to explicitly signify the lacking SymbolRepository
     * @warning As no system-wide symbol repository has been provided, this Requirement will not supply its symbols to
     *  the wider system. Logical analysis will produce unexpected results.
     */
    explicit Requirement(std::string&& name, std::string&& statement, std::string&& description, guint priority,
        std::nullptr_t system_repository);

    /**
     * @brief Create a new Requirement with the given name and register in the Glib GType system
     * @param name The initial name of the Requirement
     * @param statement The initial FOL statement of the Requirement
     * @param description The initial long-form description of the Requirement
     * @param priority The initial priority of the Requirement
     * @param system_repository The system-wide symbol repository with lifetimes guaranteed to cover that of the
     *  Requirement
     */
    explicit Requirement(std::string&& name, std::string&& statement, std::string&& description, guint priority,
        SymbolRepository& system_repository);

    /**
     * @brief Create a new Requirement with the given name and register in the Glib GType system
     * @param name The initial name of the Subsystem
     * @param statement The initial FOL statement of the Requirement
     * @param description The initial long-form description of the Requirement
     * @param priority The initial priority of the Requirement
     * @param cobject The C cast-item used by Glib::Object
     * @param builder Currently unused builder parameter to provide to the Glib::Object instance
     * @param system_repository The system-wide symbol repository with lifetimes guaranteed to cover that of the
     *  Requirement
     */
    Requirement(std::string&& name, std::string&& statement, std::string&& description, guint priority,
        BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, SymbolRepository& system_repository);

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

private:
    static log4cxx::LoggerPtr parse_logger;
    static log4cxx::LoggerPtr cnf_logger;
    static log4cxx::LoggerPtr integration_logger;

    void setup_properties(std::string &&requirement_name, std::string &&requirement_statement,
            std::string &&requirement_description, guint requirement_priority);

    static std::unique_ptr<IMutableSentence> cnf_normalise(std::unique_ptr<IMutableSentence> &&sentence);

    std::unique_ptr<SentenceRoot> populate_symbol_repository(std::unique_ptr<IMutableSentence> &&mutable_root);

    static std::string text_serialise(const IMutableSentence * sentence);

    Glib::Property<Glib::ustring> statement;

    Glib::Property<Glib::ustring> normalised_statement;

    Glib::Property<Glib::ustring> description;

    Glib::Property<guint> priority;

    std::unique_ptr<IMutableSentence> original_ast;

    std::unique_ptr<SentenceRoot> prepared_ast;

    std::optional<RepositoryBuildingVisitor> repository_building_visitor;

    std::string formatted_input_statement;

    static std::istringstream lexer_input_stream;
    static FOLLexer lexer;
    static FOLParser parser;
};

}

#endif
