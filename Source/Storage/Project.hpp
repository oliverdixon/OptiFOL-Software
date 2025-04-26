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

#include <giomm/liststore.h>

#include "IStorageObject.hpp"
#include "TreeNode.hpp"

namespace optifol
{
/**
 * @class Project
 * @brief The Project storage forms the top level of the Optifol object hierarchy; it contains many subsystems.
 */
class Project :
        public IStorageObject,
        public TreeNode
{
public:
    explicit Project(std::string&& name, const TimeT& created_time = std::chrono::system_clock::now(),
        const TimeT& last_modified_time = std::chrono::system_clock::now());

    [[nodiscard]] std::string get_identifier() const override;

    [[nodiscard]] TimeT get_creation_time() const override;

    [[nodiscard]] TimeT get_modified_time() const override;

    /**
     * @brief Tests a couple of projects for equality
     * @param other The other project
     * @return Is the current project the same as the other project?
     * @note This comparator determines equality by project metadata.
     */
    bool operator==(const Project& other) const noexcept;

    void set_identifier(const std::string& name_candidate) override;

    void set_creation_time(const TimeT& time_candidate) override;

    void set_modified_time(const TimeT& time_candidate) override;

    [[nodiscard]] std::string get_path() const override;

private:
    /**
     * @brief Time of initial creation
     * @note This quantity should be immutable in the model determined by the IStorageController, assigned only upon its
     *  initial creation.
     */
    TimeT created_time;

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
};

}

#endif
