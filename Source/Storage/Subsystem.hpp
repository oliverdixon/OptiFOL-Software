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

#include "IStorageObject.hpp"

namespace optifol
{

/**
 * @class Subsystem
 * @brief The Subsystem storage object forms the second level of the Optifol object hierarchy; it belongs to a Project,
 *  and consists of many individual requirements.
 */
class Subsystem :
    public IStorageObject
{
public:
    explicit Subsystem(std::string&& name, const TimeT& created_time = std::chrono::system_clock::now(),
        const TimeT& last_modified_time = std::chrono::system_clock::now());

    [[nodiscard]] std::string get_identifier() const override;

    [[nodiscard]] TimeT get_creation_time() const override;

    [[nodiscard]] TimeT get_modified_time() const override;

    /**
     * @brief Tests a couple of subsystem for equality
     * @param other The other subsystem
     * @return Is the current subsystem the same as the other subsystem?
     * @note This comparator determines equality by subsystem metadata.
     */
    bool operator==(const Subsystem& other) const noexcept;

    void set_identifier(const std::string &name_candidate) override;

    void set_creation_time(const TimeT& time_candidate) override;

    void set_modified_time(const TimeT& time_candidate) override;

private:
    /**
     * @brief Time of initial creation
     * @note This quantity should be immutable in the model determined by the IStorageController, assigned only upon its
     *  initial creation.
     */
    TimeT created_time;

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
};

}

#endif
