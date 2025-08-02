/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the GoogleTestDiscoveryExecutable discovery executable backend
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#ifndef GOOGLETESTDISCOVERYEXECUTABLE_HPP
#define GOOGLETESTDISCOVERYEXECUTABLE_HPP

#include "../../GUI/ProcessExecutor.hpp"
#include "DiscoveryTestExecutable.hpp"

namespace optifol
{

/**
 * @class GoogleTestDiscoveryExecutable
 * @brief Provides a non-abstract implementation of GoogleTestDiscoveryExecutable, in particular for the Google Test
 *  unit-testing framework.
 * @see GoogleTestExecutable for the runnable non-discovery equivalent.
 *
 * @details Google Test executables are queried for discovery via the asynchronous non-streaming ProcessExecutor. The
 *  executable generates a JSON-formatted structure of fixtures and test names which is parsed and stored in the
 *  internal structure. The JSON is temporarily stored on disk and deleted after parsing.
 */
class GoogleTestDiscoveryExecutable : public DiscoveryTestExecutable
{
public:
    /**
     * @brief Construct a GoogleTestDiscoveryExecutable for a Google Test executable at the given path.
     * @param executable_path The file-system path, relative to the CWD of the executable, to the Google Test
     *  executable.
     */
    explicit GoogleTestDiscoveryExecutable(const Glib::ustring &executable_path);

    /**
     * @brief Construct a GoogleTestDiscoveryExecutable for a Google Test executable at the given path.
     * @param executable_path The file-system path, relative to the CWD of the executable, to the Google Test
     *  executable.
     * @param cobject Glib C object
     * @param builder Gtk::Builder object
     */
    GoogleTestDiscoveryExecutable(
            const Glib::ustring &executable_path, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

private:
    /**
     * @brief Initiates discovery process on the loaded Google Test executable. A temporary file is created for the
     *  asynchronously launched process to write a JSON-formatted structure describing the test structure.
     */
    void start_discovery();

    /**
     * @brief Handles the end of the Google Test discovery sub-process: parses the JSON and and deleted the temporary
     *  file.
     * @param exit_code The exit code of the executable.
     */
    void discovery_done_callback(int exit_code);

    std::unique_ptr<ProcessExecutor> discovery_executor;
    std::optional<std::string> discovery_tmp_file_path;
};

} // namespace optifol

#endif // GOOGLETESTDISCOVERYEXECUTABLE_HPP
