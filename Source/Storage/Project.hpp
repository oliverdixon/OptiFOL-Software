/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the project-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "IStorageObject.hpp"

namespace optifol
{
/**
 * @class Project
 * @brief The Project storage forms the top level of the OptiFOL object hierarchy; it contains many subsystems.
 */
class Project :
        public IStorageObject
{
public:
    Project(std::size_t id, const std::string& name, const time_t& created_time = {},
        const time_t& last_modified_time = {});

    [[nodiscard]] std::string get_identifier() const override;

    [[nodiscard]] time_t get_creation_time() const override;

    [[nodiscard]] time_t get_modified_time() const override;

    [[nodiscard]] std::size_t get_controller_id() const noexcept override;

    /**
     * @brief Tests a couple of projects for surface-level equality
     * @param other The other project
     * @return Is the current project the same as the other project?
     * @note This comparator determines equality by project metadata.
     */
    bool operator==(const Project& other) const;

    bool operator==(std::size_t other_id) const;

private:
    const time_t created_time;
    time_t last_modified_time;
    std::string name;
    std::size_t id;
};

}

#endif
