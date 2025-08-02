/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the GoogleTestExecutable executable backend
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#ifndef GOOGLETESTEXECUTABLE_HPP
#define GOOGLETESTEXECUTABLE_HPP

#include "TargetTestExecutableBase.hpp"

namespace optifol
{

/**
 * @class GoogleTestExecutable
 * @brief Provides a non-abstract backend for the TargetTestExecutableBase, in particular for test executables provided
 *  by Google Test.
 * @see GoogleTestDiscoveryExecutable for the non-runnable discovery equivalent.
 */
class GoogleTestExecutable : public TargetTestExecutableBase
{
public:
    /**
     * @brief Create a new GoogleTestExecutable with the given executable path.
     * @param executable_path The path of the Google Test executable.
     */
    explicit GoogleTestExecutable(const Glib::ustring &executable_path);

    /**
     * @brief Create a new GoogleTestExecutable with the given executable path.
     * @param executable_path The path of the Google Test executable.
     * @param cobject Glib C object
     * @param builder Gtk::Builder reference
     */
    GoogleTestExecutable(const Glib::ustring &executable_path, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder);
};

} // namespace optifol

#endif // GOOGLETESTEXECUTABLE_HPP
