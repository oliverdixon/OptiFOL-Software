/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification of the PostgreSQL storage engine backend
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#ifndef PGDATABASECONTROLLER_HPP
#define PGDATABASECONTROLLER_HPP

#include <simdjson.h>
#include <string>
#include <pqxx/connection>

#include "IStorageController.hpp"
#include "PGUpdateNotification.hpp"
#include "../LegacyWrappers.hpp"
#include "Project/PGProjectModel.hpp"
#include "Project/ProjectHierarchicalModel.hpp"
#include "Requirement/PGRequirementModel.hpp"
#include "Subsystem/PGSubsystemModel.hpp"

namespace optifol
{
/**
 * @class PGDatabaseController
 * @brief The PGDatabaseController provides a PostgreSQL-based backend storage engine for OptiFOL
 */
class PGDatabaseController :
        public IStorageController
{
public:
    /**
     * @brief Establish a connection with the database at the given URI.
     * @param db_uri URI of the PostgreSQL database, beginning with the 'postgresql://' protocol specifier.
     * @throws StorageConnectionException The connection could not be successfully established
     * @post The connection and all dependent concrete containers have been established and is maintained in the
     *  internal controller model
     */
    explicit PGDatabaseController(const std::string& db_uri);

    /**
     * @brief Close the database connection.
     * @todo We should close any open transactions here, and flush any changes in the locally cached storage objects.
     */
    ~PGDatabaseController() override;

    /**
     * @copybrief IStorageController::update
     * @throws BadStorageNotificationException A propagation update was received from the DB, but it was malformed.
     */
    void update() override;

    /**
     * @copybrief IStorageController::build_project_hierarchical_model
     * @details This builder creates a new hierarchical project view synchronised to the contents of the storable
     *  objects in the present database controller instance. The synchronisation is two-way, such that the built
     *  hierarchical model may update the database model of changes, and vise versa. This function should be tested
     *  extensively to verify absence of scoping issues, noting that all signal-trackers support automatic signal
     *  disconnection via the libsigc++ framework.
     * @return A Glib-wrapped project hierarchical model, suitable for use with GTK components.
     */
    [[nodiscard]] Glib::RefPtr<ProjectHierarchicalModel> build_project_hierarchical_model() const override;

private:
    /**
     * @brief Parse and delegate the changes described by a JSON-formatted WAL replication message from the DB
     * @param payload The JSON string posted to the WAL slot
     */
    void despatch_json_change(std::string_view payload);

    /**
     * @brief Action the changes described by a JSON-formatted WAL replication message from the DB
     * @tparam Type The StorageType scope target described by the WAL message
     * @param notification The DB notification object
     */
    template<StorableType Type>
    void handle_change(const PGUpdateNotification& notification) const = delete;

    /**
     * @brief Explicitly specialised convenience function to add insertion, update, and deletion listeners to a given
     *  hierarchical storage type such that its data is kept synchronised with the instances of the models detained by
     *  the database controller.
     * @tparam HierarchicalStorageType The type of hierarchical storage type to which listeners should be assigned
     * @param hierarchical_model The hierarchical model to which listeners should be assigned
     */
    template<typename HierarchicalStorageType>
    void add_all_listeners(HierarchicalStorageType& hierarchical_model) const = delete;

    const std::string wal_slot_name{"optifol_" + std::to_string(LegacyWrappers::get_pid())};

    simdjson::ondemand::parser json_parser;

    std::optional<pqxx::connection> connection;

    std::unique_ptr<PGProjectModel> project_model;

    std::unique_ptr<PGSubsystemModel> subsystem_model;

    std::unique_ptr<PGRequirementModel> requirement_model;
};

template<>
inline void PGDatabaseController::handle_change<Project>(const PGUpdateNotification& notification) const
{
    assert(notification.scope == PGUpdateNotification::Scope::Project);

    switch (notification.action) {
    case PGUpdateNotification::Action::NoOp: break;
    case PGUpdateNotification::Action::Insert:
        project_model->enqueue_inbound_load(notification.id);
        break;
    case PGUpdateNotification::Action::Update:
        project_model->enqueue_inbound_reload(notification.id);
        break;
    case PGUpdateNotification::Action::Delete:
        project_model->enqueue_inbound_unload(notification.id);
        break;
    }
}

template<>
inline void PGDatabaseController::handle_change<Subsystem>(const PGUpdateNotification& notification) const
{
    assert(notification.scope == PGUpdateNotification::Scope::Subsystem);

    switch (notification.action) {
    case PGUpdateNotification::Action::NoOp: break;
    case PGUpdateNotification::Action::Insert:
        subsystem_model->enqueue_inbound_load(notification.id);
        break;
    case PGUpdateNotification::Action::Update:
        subsystem_model->enqueue_inbound_reload(notification.id);
        break;
    case PGUpdateNotification::Action::Delete:
        subsystem_model->enqueue_inbound_unload(notification.id);
        break;
    }
}

template<>
inline void PGDatabaseController::handle_change<Requirement>(const PGUpdateNotification& notification) const
{
    assert(notification.scope == PGUpdateNotification::Scope::Requirement);

    switch (notification.action) {
    case PGUpdateNotification::Action::NoOp: break;
    case PGUpdateNotification::Action::Insert:
        requirement_model->enqueue_inbound_load(notification.id);
        break;
    case PGUpdateNotification::Action::Update:
        requirement_model->enqueue_inbound_reload(notification.id);
        break;
    case PGUpdateNotification::Action::Delete:
        requirement_model->enqueue_inbound_unload(notification.id);
        break;
    }
}

template<>
inline void PGDatabaseController::add_all_listeners(ProjectHierarchicalModel& hierarchical_model) const
{
    project_model->add_insert_subscriber(sigc::mem_fun(hierarchical_model,
        &ProjectHierarchicalModel::register_object), true);
    hierarchical_model.add_insert_subscriber(sigc::mem_fun(*project_model,
        &PGProjectModel::register_object), false);

    project_model->add_delete_subscriber(sigc::mem_fun(hierarchical_model,
        &ProjectHierarchicalModel::remove_object));
    hierarchical_model.add_delete_subscriber(sigc::mem_fun(*project_model, &PGProjectModel::remove_object));
}

template<>
inline void PGDatabaseController::add_all_listeners(SubsystemHierarchicalModel& hierarchical_model) const
{
    subsystem_model->add_insert_subscriber(sigc::mem_fun(hierarchical_model,
        &SubsystemHierarchicalModel::register_object), true);
    hierarchical_model.add_insert_subscriber(sigc::mem_fun(*subsystem_model,
        &PGSubsystemModel::register_object), false);

    subsystem_model->add_delete_subscriber(sigc::mem_fun(hierarchical_model,
        &SubsystemHierarchicalModel::remove_object));
    hierarchical_model.add_delete_subscriber(sigc::mem_fun(*subsystem_model, &PGSubsystemModel::remove_object));
}

template<>
inline void PGDatabaseController::add_all_listeners(RequirementHierarchicalModel& hierarchical_model) const
{
    requirement_model->add_insert_subscriber(sigc::mem_fun(hierarchical_model,
        &RequirementHierarchicalModel::register_object), true);
    hierarchical_model.add_insert_subscriber(sigc::mem_fun(*requirement_model,
        &PGRequirementModel::register_object), false);

    requirement_model->add_delete_subscriber(sigc::mem_fun(hierarchical_model,
        &RequirementHierarchicalModel::remove_object));
    hierarchical_model.add_delete_subscriber(sigc::mem_fun(*requirement_model, &PGRequirementModel::remove_object));
}

}

#endif
