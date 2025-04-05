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

#include "Requirement.hpp"
#include "../IStorableObjectModel.hpp"
#include "../PGStorableObjectModelBase.hpp"
#include "../Subsystem/Subsystem.hpp"

namespace optifol
{

class PGRequirementModel :
        public PGStorableObjectModelBase<Requirement>,
        public IStorableObjectModel<Requirement>
{
public:
    explicit PGRequirementModel(pqxx::connection& connection);

    [[nodiscard]] std::size_t get_item_count() const noexcept override;

    void register_object(Glib::RefPtr<Requirement>&& requirement) override;

    [[nodiscard]] Glib::RefPtr<Requirement> get_object(const Requirement& requirement) override;

    [[nodiscard]] Glib::RefPtr<Requirement> get_object(std::size_t requirement_id) override;

    void remove_object(const Requirement& requirement) override;

    void remove_object(std::size_t requirement_id) override;

    void load_for_subsystem(Glib::RefPtr<Subsystem>&& subsystem);

private:
    struct DBFieldIdx
    {
        static constexpr pqxx::row_size_type ID = 0;
        static constexpr pqxx::row_size_type SubsystemID = 1;
        static constexpr pqxx::row_size_type Name = 2;
        static constexpr pqxx::row_size_type CreatedAt = 3;
        static constexpr pqxx::row_size_type LastModified = 4;
        static constexpr pqxx::row_size_type Sentence = 5;
        static constexpr pqxx::row_size_type Priority = 6;
        static constexpr pqxx::row_size_type Description = 7;
        static constexpr pqxx::row_size_type TestID = 8;
    };

    [[nodiscard]] pqxx::result filter_objects(const std::ostringstream &sql_parameter, std::size_t maximum_return_count)
        const override;

    void emplace_object(const pqxx::row& row) override;

    void deplace_object(std::size_t id) override;
};

}

#endif
