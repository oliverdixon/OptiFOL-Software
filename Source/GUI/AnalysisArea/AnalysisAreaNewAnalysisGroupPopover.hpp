/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Analysis Area "New Analysis Group" popover manager
 * @author Oliver Dixon
 * @date 2025-07-09
 * @version Development
 */

#ifndef ANALYSISAREANEWANALYSISGROUPPOPOVER_HPP
#define ANALYSISAREANEWANALYSISGROUPPOPOVER_HPP

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/popover.h>
#include <log4cxx/logger.h>

namespace optifol
{

class AnalysisArea;

/**
 * @class AnalysisAreaNewAnalysisGroupPopover
 * @brief Manage the <i>New Analysis Group</i> popover for the Analysis area
 * @see AnalysisArea for the parent area
 * 
 * @details
 *  The <i>New Analysis Group</i> popover provides controls for creating a new AnalysisGroup, consisting of one or more
 *  Requirement objects from the active Subsystem, to be subject to mathematical analysis. The following GTK elements
 *  are expected from the given Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>new_analysis_group_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>new_analysis_group_confirm</code></td>
 *          <td>Confirm creation of a new AnalysisGroup</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>new_analysis_group_cancel</code></td>
 *          <td>Cancels creation of a new AnalysisGroup</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>new_analysis_group_property_name</code></td>
 *          <td>Text entry area for the name of the new AnalysisGroup</td>
 *      </tr>
 *  </table>
 *  A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in the expected
 *  type instantiations.
 */
class AnalysisAreaNewAnalysisGroupPopover
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param analysis_area An observing reference to the view of which the popover is a member
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    AnalysisAreaNewAnalysisGroupPopover(Gtk::Builder& builder, const AnalysisArea& analysis_area);

private:
    /**
     * @brief Handle a click of the <i>Confirm</i> by attempting to create the named AnalysisGroup.
     */
    void confirm_button_clicked() const;

    /**
     * @brief Handle a click of the <i>Cancel</i> button by discarding all input and closing the popover.
     */
    void cancel_button_clicked() const;

    /**
     * @brief Clear all user fields in the popover
     */
    void clear_inputs() const;

    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    const AnalysisArea& analysis_area;
    Gtk::Popover * const my_popover;
    Gtk::Button * const confirm_button;
    Gtk::Button * const cancel_button;
    Gtk::Entry * const name_entry;
};

} // namespace optifol

#endif // ANALYSISAREANEWANALYSISGROUPPOPOVER_HPP
