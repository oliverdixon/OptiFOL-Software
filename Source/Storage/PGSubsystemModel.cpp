/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "PGSubsystemModel.hpp"

#include <iostream>

namespace optifol
{

PGSubsystemModel::PGSubsystemModel(pqxx::connection &connection) :
    PGStorableObjectModel(connection)
{
}

void PGSubsystemModel::load()
{
    std::cout << "Waiting to load: ";

    while (!load_queue.empty()) {
        const auto pending_id = load_queue.front();
        load_queue.pop();
        std::cout << pending_id << ' ';
    }

    std::cout << std::endl;
}

void PGSubsystemModel::reload()
{
    std::cout << "Waiting to reload: ";

    while (!reload_queue.empty()) {
        const auto pending_id = reload_queue.front();
        reload_queue.pop();
        std::cout << pending_id << ' ';
    }

    std::cout << std::endl;
}

void PGSubsystemModel::unload()
{
    std::cout << "Waiting to unload: ";

    while (!unload_queue.empty()) {
        const auto pending_id = unload_queue.front();
        unload_queue.pop();
        std::cout << pending_id << ' ';
    }

    std::cout << std::endl;
}

}
