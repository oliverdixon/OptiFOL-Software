/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed requirement storage container
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#ifndef PGREQUIREMENTMODEL_HPP
#define PGREQUIREMENTMODEL_HPP

#include "IRequirementModel.hpp"
#include "../PGStorableObjectModelBase.hpp"

namespace optifol
{

class PGRequirementModel :
        public PGStorableObjectModelBase<Requirement>,
        public IRequirementModel
{
public:
    explicit PGRequirementModel(pqxx::connection& connection);

    [[nodiscard]] std::size_t get_item_count() const noexcept override;

    void register_requirement(Glib::RefPtr<Requirement>&& requirement) override;

    [[nodiscard]] Glib::RefPtr<Requirement> get_requirement(const Requirement& requirement) override;

    [[nodiscard]] Glib::RefPtr<Requirement> get_requirement(std::size_t requirement_id) override;

    void remove_requirement(const Requirement& requirement) override;

    void remove_requirement(std::size_t requirement_id) override;

private:
    [[nodiscard]] pqxx::result filter_objects(const std::ostringstream &sql_parameter, std::size_t maximum_return_count)
        const override;

    void emplace_object(const pqxx::row& row) override;

    void deplace_object(std::size_t id) override;
};

}

#endif
