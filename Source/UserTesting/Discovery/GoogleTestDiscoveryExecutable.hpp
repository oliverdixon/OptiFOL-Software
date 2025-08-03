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

#include <rapidjson/document.h>

#include "../../GUI/ProcessExecutor.hpp"
#include "DiscoveryTestExecutable.hpp"

namespace optifol
{

/**
 * @class GoogleTestDiscoveryExecutable
 * @brief Provides a non-abstract implementation of DiscoveryTestExecutable for the Google Test unit-testing framework.
 *
 * @details Google Test executables are queried for discovery via the asynchronous non-streaming ProcessExecutor. The
 *  executable generates a JSON-formatted structure of fixtures and test names which is parsed and stored in the
 *  internal structure. The JSON is temporarily stored on disk and is deleted after parsing. As an RAII class, discovery
 *  (and subsequent subprocess invocation) is performed during construction.
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

    /**
     * @brief Destruct the GoogleTestDiscoveryExecutable, logging errors if there is an unresolved sub-process.
     */
    ~GoogleTestDiscoveryExecutable() override;

private:
    /**
     * @brief Initiates discovery process on the loaded Google Test executable. A temporary file is created for the
     *  asynchronously launched process to write a JSON-formatted structure describing the test structure.
     */
    void start_discovery();

    /**
     * @brief Handles the end of the Google Test discovery sub-process: parses the JSON into the fixture/test model of
     *  the DiscoveryTestExecutable base class, and and deletes the temporary file.
     * @param exit_code The exit code of the executable.
     * @pre @ref discovery_tmp_file_path is populated.
     */
    void discovery_done_callback(int exit_code) noexcept;

    /**
     * @brief Parse the JSON payload produced by Google Test, parsed by RapidJSON into the given document.
     * @param document The parsed DOM of the JSON payload.
     * @throws ParseError if the document schema was invalid.
     */
    void parse_json_payload(const rapidjson::Document &document) const;

    static const log4cxx::LoggerPtr logger;

    std::unique_ptr<ProcessExecutor> discovery_executor;
    std::optional<std::string> discovery_tmp_file_path;
};

} // namespace optifol

#endif // GOOGLETESTDISCOVERYEXECUTABLE_HPP
