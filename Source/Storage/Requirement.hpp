/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the requirement-level storage object
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#ifndef REQUIREMENT_HPP
#define REQUIREMENT_HPP

#include "IStorageObject.hpp"

namespace optifol
{

class Requirement :
        public IStorageObject
{
public:
    Requirement(std::string&& name, const TimeT& created_time,
        const TimeT& last_modified_time, std::string&& statement, std::size_t priority, std::string&& description,
        std::optional<std::size_t> test_id, std::size_t stakeholder);

    [[nodiscard]] std::string get_identifier() const override;

    [[nodiscard]] TimeT get_creation_time() const override;

    [[nodiscard]] TimeT get_modified_time() const override;

    /**
     * @brief Tests a couple of requirements for equality
     * @param other The other requirement
     * @return Is the current requirement the same as the other requirement?
     * @note This comparator determines equality by requirement metadata.
     */
    bool operator==(const Requirement& other) const noexcept;

    [[nodiscard]] std::string get_statement() const;

    [[nodiscard]] std::size_t get_priority() const;

    [[nodiscard]] std::string get_description() const;

    [[nodiscard]] std::optional<std::size_t> get_test() const;

    [[nodiscard]] std::size_t get_stakeholder() const;

    void set_identifier(const std::string &name_candidate) override;

    void set_creation_time(const TimeT& time_candidate) override;

    void set_modified_time(const TimeT& time_candidate) override;

    void set_description(const std::string &description_candidate);

    void set_priority(std::size_t new_priority);

    void set_statement(const std::string &statement_candidate);

    void attempt_set_priority(const std::string &priority_candidate);

    void set_test(const std::string &test_candidate);

    void set_stakeholder(const std::string &stakeholder_candidate);

private:
    /**
     * @brief Time of initial creation
     * @note This quantity should be immutable in the model determined by the IStorageController, assigned only upon its
     *  initial creation.
     */
    TimeT created_time;

    /**
     * @brief Time of most recent mutation
     * @note This quantity should be updated upon the changing of requirement metadata, or the changing of any data held
     *  by the requirement, such as any of its constituent Subsystems or their requirements.
     */
    TimeT last_modified_time;

    /**
     * @brief The human-readable name of the requirement
     */
    std::string name;

    std::string statement;

    std::size_t priority;

    std::string description;

    // TODO: reference actual test (UML 1-1, optional)
    std::optional<std::size_t> test_id;

    // TODO: reference actual stakeholder(s) (UML M-M)
    std::size_t stakeholder;
};

}

#endif
