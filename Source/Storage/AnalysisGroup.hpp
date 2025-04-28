/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for an analysis grouping of requirements
 * @author Oliver Dixon
 * @date 2025-04-27
 * @version Development
 */

#ifndef ANALYSISGROUP_HPP
#define ANALYSISGROUP_HPP

#include <giomm/liststore.h>

#include "Requirement.hpp"
#include "StorageObjectBase.hpp"

namespace optifol
{

class AnalysisGroup :
        public StorageObjectBase
{
public:
    /**
     * @brief Create a new Analysis Group with the given name and register in the Glib GType system
     * @param name The initial name of the Analysis Group
     */
    explicit AnalysisGroup(std::string&& name);

    AnalysisGroup(std::string &&name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    // TODO: shouldn't be public
    Glib::RefPtr<Gio::ListStore<Requirement>> requirements = Gio::ListStore<Requirement>::create();
};

}

#endif
