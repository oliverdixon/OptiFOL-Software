/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the subsystem-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#ifndef SUBSYSTEM_HPP
#define SUBSYSTEM_HPP

#include "../IStorageObject.hpp"

namespace optifol
{

/**
 * @class Subsystem
 * @brief The Subsystem storage object forms the second level of the OptiFOL object hierarchy; it belongs to a Project,
 *  and consists of many individual requirements.
 */
class Subsystem :
    public IStorageObject
{
public:
    Subsystem(std::size_t id, std::string&& name, const TimeT& created_time, const TimeT& last_modified_time);

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
    bool operator==(const Subsystem& other) const noexcept;

    bool operator==(std::size_t other_id) const noexcept override;

    void set_identifier(const std::string &name_candidate) override;

private:
    /**
     * @brief Time of initial creation
     * @note This quantity should be immutable in the model determined by the IStorageController, assigned only upon its
     *  initial creation.
     */
    const TimeT created_time;

    /**
     * @brief Time of most recent mutation
     * @note This quantity should be updated upon the changing of Subsystem metadata, or the changing of any data held
     *  by the Subsystem, such as any of its constituent requirements.
     */
    TimeT last_modified_time;

    /**
     * @brief The human-readable name of the Project
     */
    std::string name;

    /**
     * @brief The numerical ID of the Subsystem, unique up to being the IStorageController primary key for the Subsystem
     *  entity
     */
    const std::size_t id;
};

}

#endif
