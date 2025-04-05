/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed subsystem storage container
 * @author Oliver Dixon
 * @date 2025-03-18
 * @version Development
 */

#ifndef PGSUBSYSTEMMODEL_HPP
#define PGSUBSYSTEMMODEL_HPP

#include "Subsystem.hpp"
#include "../IStorableObjectModel.hpp"
#include "../PGStorableObjectModelBase.hpp"

namespace optifol
{

/**
 * @class PGSubsystemModel
 * @brief TODO
 */
class PGSubsystemModel :
        public PGStorableObjectModelBase<Subsystem>,
        public IStorableObjectModel<Subsystem>
{
public:
    /**
     * @brief Construct a subsystem cache container
     * @param connection The established PostgreSQL database connection
     */
    explicit PGSubsystemModel(pqxx::connection& connection);

    [[nodiscard]] std::size_t get_item_count() const noexcept override;

    void register_object(Glib::RefPtr<Subsystem>&& subsystem) override;

    [[nodiscard]] Glib::RefPtr<Subsystem> get_object(const Subsystem& subsystem) override;

    [[nodiscard]] Glib::RefPtr<Subsystem> get_object(std::size_t subsystem_id) override;

    void remove_object(const Subsystem& subsystem) override;

    void remove_object(std::size_t subsystem_id) override;

private:
    pqxx::result filter_objects(const std::ostringstream &sql_parameter, std::size_t maximum_return_count) const
        override;

    void emplace_object(const pqxx::row& row) override;

    void deplace_object(std::size_t id) override;
};

}

#endif
