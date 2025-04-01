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

#include "../IStorageObject.hpp"

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
    Project(std::size_t id, std::string&& name, const TimeT& created_time, const TimeT& last_modified_time);

    [[nodiscard]] std::string get_identifier() const override;

    [[nodiscard]] TimeT get_creation_time() const override;

    [[nodiscard]] TimeT get_modified_time() const override;

    [[nodiscard]] std::size_t get_controller_id() const noexcept override;

    /**
     * @brief Tests a couple of projects for surface-level equality
     * @param other The other project
     * @return Is the current project the same as the other project?
     * @note This comparator determines equality by project metadata.
     */
    bool operator==(const Project& other) const noexcept;

    bool operator==(std::size_t other_id) const noexcept override;

    void set_identifier(const std::string& name_candidate) override;

private:
    /**
     * @brief Time of initial creation
     * @note This quantity should be immutable in the model determined by the IStorageController, assigned only upon its
     *  initial creation.
     */
    const TimeT created_time;

    /**
     * @brief Time of most recent mutation
     * @note This quantity should be updated upon the changing of Project metadata, or the changing of any data held by
     *  the Project, such as any of its constituent Subsystems or their requirements.
     */
    TimeT last_modified_time;

    /**
     * @brief The human-readable name of the Project
     */
    std::string name;

    /**
     * @brief The numerical ID of the Project, unique up to being the IStorageController primary key for the Project
     *  entity
     */
    const std::size_t id;
};

}

#endif
