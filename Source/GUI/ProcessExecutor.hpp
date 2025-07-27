/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the external non-streaming Process Executor
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#ifndef TESTEXECUTORBASE_HPP
#define TESTEXECUTORBASE_HPP

#include <log4cxx/logger.h>
#include <glibmm/spawn.h>
#include <string>
#include <vector>

namespace optifol
{

/**
 * @class ProcessExecutor
 * @brief Provides a RAII container for asynchronous execution of external processes under the Glib framework.
 * @details The ProcessExecutor wraps the @ref Glib::spawn_async_with_pipes in an RAII container and provides a callback
 *  option to notify users of subprocess termination.
 */
class ProcessExecutor : public sigc::trackable
{
public:
    /**
     * @brief Asynchronously execute an external process at the given working directory, with the given argument vector,
     *  optionally including a key-value environment specification and exit callback.
     * @param working_directory The working directory in which the sub-process should be started.
     * @param argv The argument vector, where the first entry is the command to execute. The system PATH will be
     *  searched to find the executable, but it is not run under a shell.
     * @param envp The optional <code>KEY=VALUE</code> set of environment variables to export into the sub-process
     *  environment.
     * @param finished_callback The noexcept callback to execute on completion of the sub-process, taking the numerical
     *  exit code.
     */
    ProcessExecutor(const std::string &working_directory, const std::vector<std::string> &argv,
        const std::vector<std::string> &envp = {}, sigc::slot<void(int)>&& finished_callback = {});

    // ReSharper disable once CppHidingFunction
    /*
     * A well-document and understood sigc++ constraint prevents deletions through sigc::trackable pointers. Hiding the
     * non-virtual ~sigc::trackable is a non-issue for our API use-case.
     */
    /**
     * @brief Destruct the ProcessExecutor, disconnecting any signals.
     */
    virtual ~ProcessExecutor() = default;

protected:
    /**
     * @brief Reap the child process identified with the given PID, and invoke the user-supplied @ref finished_callback.
     * @param ended_pid The PID of the ended process.
     * @param exit_code The numerical exit code of the process; anything except zero is deemed indicative of an error.
     */
    virtual void reap_child(Glib::Pid ended_pid, int exit_code) noexcept;

    int stdout_fd = -1;
    int stderr_fd = -1;
    Glib::Pid pid = -1;

private:
    static const log4cxx::LoggerPtr logger;

    const sigc::slot<void(int)> finished_callback;
};

} // namespace optifol

#endif // TESTEXECUTORBASE_HPP
