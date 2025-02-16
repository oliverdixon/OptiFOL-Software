/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation of the PostgreSQL storage engine backend
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#include <pqxx/pqxx>

#include "PGDatabase.hpp"
#include "../Exceptions/StorageConnectionException.hpp"

namespace optifol
{

PGDatabase::PGDatabase(const std::string& db_uri)
{
    try {
        connection = std::make_unique<pqxx::connection>(db_uri);
    } catch (const pqxx::failure& exception) {
        throw StorageConnectionException(exception.what());
    }
}

std::vector<std::string> PGDatabase::get_project_names()
{
    pqxx::work tx{*connection};
    const pqxx::result result{tx.exec("SELECT name FROM Project")};
    std::vector<std::string> project_names;
    project_names.reserve(static_cast<std::size_t>(result.size()));

    for (const auto& row : result)
        project_names.emplace_back(row["name"].c_str());

    tx.commit(); // Not necessary, but best to be consistently explicit.
    return project_names;
}

}
