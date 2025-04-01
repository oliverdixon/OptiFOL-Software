/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 4/1/25.
//

#ifndef REQUIREMENTHIERARCHICALMODEL_HPP
#define REQUIREMENTHIERARCHICALMODEL_HPP

#include <giomm/liststore.h>

#include "IRequirementModel.hpp"

namespace optifol
{

class RequirementHierarchicalModel :
        public IRequirementModel,
        public Gio::ListStore<Requirement>
{
public:
    void register_requirement(Glib::RefPtr<Requirement>&& requirement) override;

    [[nodiscard]] Glib::RefPtr<Requirement> get_requirement(const Requirement& requirement) override;

    [[nodiscard]] Glib::RefPtr<Requirement> get_requirement(std::size_t requirement_id) override;

    void remove_requirement(const Requirement& requirement) override;

    void remove_requirement(std::size_t requirement_id) override;

private:
    std::optional<guint> find_requirement_position(const Requirement& requirement) const;

    std::optional<guint> find_requirement_position(std::size_t requirement_id) const;

    Glib::RefPtr<Requirement> dummy_requirement = Glib::make_refptr_for_instance(new Requirement(0, {}, {}, {}, {}, 0,
        {}, 0, 0));
};

}

#endif
