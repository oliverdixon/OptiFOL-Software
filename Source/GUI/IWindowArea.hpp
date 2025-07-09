/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Interface specification for the Subsystem-sensitive Window Area
 * @author Oliver Dixon
 * @date 2025-07-09
 * @version Development
 */

#ifndef IWINDOWAREA_HPP
#define IWINDOWAREA_HPP

#include <glibmm/refptr.h>
#include <sigc++/trackable.h>

namespace optifol
{

class Subsystem;

/**
 * @class IWindowArea
 * @brief A window area is a loosely defined "major" area of the Optifol GUI that is largely dependent on the presently
 *  selected model in the ProjectHierarchyPane. It is trackable to support the usage of its member functions as
 *  libsigc++ callbacks.
 */
class IWindowArea : public sigc::trackable
{
public:
    // ReSharper disable once CppHidingFunction
    /*
     * A well-document and understood sigc++ constraint prevents deletions through sigc::trackable pointers. Hiding the
     * non-virtual ~sigc::trackable is a non-issue for our API use-case.
     */
    /**
     * @brief Destruct the window area
     */
    virtual ~IWindowArea() = default;

    /**
     * @brief Handle a change in the present selection to a new Subsystem model
     * @param subsystem_model The newly selected Subsystem model
     * @see ProjectHierarchyPane::add_subsystem_change_callback
     */
    virtual void select_model(const Glib::RefPtr<const Subsystem> &subsystem_model) = 0;

    /**
     * @brief Handle a deselection (and no re-selection) of the previously selected Subsystem model
     * @see ProjectHierarchyPane::add_subsystem_change_callback
     */
    virtual void deselect_model() = 0;
};

} // namespace optifol

#endif // IWINDOWAREA_HPP
