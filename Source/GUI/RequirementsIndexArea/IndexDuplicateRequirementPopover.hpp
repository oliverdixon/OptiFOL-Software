/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Duplicate Requirement UI popover
 * @author Oliver Dixon
 * @date 2025-07-10
 * @version Development
 */

#ifndef INDEXDUPLICATEREQUIREMENTPOPOVER_HPP
#define INDEXDUPLICATEREQUIREMENTPOPOVER_HPP

#include <gtkmm/builder.h>

namespace optifol
{

class RequirementsIndexArea;

class IndexDuplicateRequirementPopover
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param index_area A mutating reference to the view of which the popover is a member
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    IndexDuplicateRequirementPopover(Gtk::Builder& builder, RequirementsIndexArea& index_area);
};

} // namespace optifol

#endif // INDEXDUPLICATEREQUIREMENTPOPOVER_HPP
