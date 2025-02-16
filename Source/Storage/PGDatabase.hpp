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

#ifndef PGDATABASE_HPP
#define PGDATABASE_HPP

#include <string>
#include <pqxx/connection>

#include "IStorageController.hpp"

namespace optifol
{

/**
 * @class PGDatabase
 * @brief The PGDatabase provides a PostgreSQL-based backend storage engine for OptiFOL
 */
class PGDatabase :
        public IStorageController
{
public:
    /**
     * @brief Establish a connection with the database at the given URI.
     * @param db_uri URI of the PostgreSQL database, beginning with the 'postgresql://' protocol specifier.
     * @throws StorageConnectionException The connection could not be successfully established
     */
    explicit PGDatabase(const std::string& db_uri);

    /**
     * @brief Retrieve a list of all project names in the opened database
     * @return The list of all project names in the opened database
     */
    [[nodiscard]] std::vector<std::string> get_project_names() override;

private:
    std::unique_ptr<pqxx::connection> connection = nullptr;
};

}

#endif
