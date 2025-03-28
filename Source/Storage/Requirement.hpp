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
    Requirement(std::size_t id, const std::string& name, const TimeT& created_time, const TimeT& last_modified_time,
        const std::string& statement, std::size_t priority);

    [[nodiscard]] std::string get_identifier() const override;

    [[nodiscard]] TimeT get_creation_time() const override;

    [[nodiscard]] TimeT get_modified_time() const override;

    [[nodiscard]] std::size_t get_controller_id() const noexcept override;

    /**
     * @brief Tests a couple of requirements for surface-level equality
     * @param other The other requirement
     * @return Is the current requirement the same as the other requirement?
     * @note This comparator determines equality by requirement metadata.
     */
    bool operator==(const Requirement& other) const noexcept;

    bool operator==(std::size_t other_id) const noexcept override;

    [[nodiscard]] std::string get_statement() const;

    [[nodiscard]] std::size_t get_priority() const;

private:
    /**
     * @brief Time of initial creation
     * @note This quantity should be immutable in the model determined by the IStorageController, assigned only upon its
     *  initial creation.
     */
    const TimeT created_time;

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

    /**
     * @brief The numerical ID of the requirement, unique up to being the IStorageController primary key for the
     *  requirement entity
     */
    const std::size_t id;

    /*
     * TODO: put an actual statement in here. Will need to implement a PQXX "parser" to convert a sentence to the IR
     *  model. We could do this naively by passing the string to Flex, but could we devise a better binary format? But
     *  that would make error correction more difficult. Need to profile: does removing the lexer but keeping the parser
     *  give us significant gains? If so, just store in an equivalent binary format that can be trivially lexed and send
     *  to the parser for validation. But I suspect that Bison is taking up most of the time anyway, so I don't think it
     *  would be worth the extra effort. Just something to think about later down the road... How much EC is suitable
     *  for a DB? Can we trust our own DB? Could we add hashes?
     */
    std::string statement;

    std::size_t priority;
};

}

#endif
