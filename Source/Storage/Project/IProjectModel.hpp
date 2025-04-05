/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 3/30/25.
//

#ifndef IPROJECTMODEL_HPP
#define IPROJECTMODEL_HPP

#include "Project.hpp"
#include "../IAbstractModel.hpp"

namespace optifol
{

class IProjectModel :
        public IAbstractModel
{
public:
    virtual void register_project(Glib::RefPtr<Project>&& project) = 0;

    [[nodiscard]] virtual Glib::RefPtr<Project> get_project(const Project& project) = 0;

    [[nodiscard]] virtual Glib::RefPtr<Project> get_project(std::size_t project_id) = 0;

    virtual void remove_project(const Project& project) = 0;

    virtual void remove_project(std::size_t project_id) = 0;
};

}

#endif
