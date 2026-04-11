/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the external Process Executor
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#include <glibmm/main.h>

#include "../Logging.hpp"
#include "ProcessExecutor.hpp"

namespace optifol
{

const log4cxx::LoggerPtr ProcessExecutor::logger =
        Logging::get_logger({"SubprocessControl", "ProcessExecutor"});

ProcessExecutor::ProcessExecutor(const std::string &working_directory, const std::vector<std::string> &argv,
        const std::vector<std::string> &envp, sigc::slot<void(int)> &&finished_callback) :
    finished_callback(std::move(finished_callback))
{
    try {
        Glib::spawn_async_with_pipes(working_directory, argv, envp,
                Glib::SpawnFlags::SEARCH_PATH | Glib::SpawnFlags::DO_NOT_REAP_CHILD, {}, &pid, nullptr,
                &stdout_fd, &stderr_fd);
    } catch (const Glib::SpawnError &spawn_error) {
        logger->error("Failed to spawn sub-process.");
        logger->error(spawn_error.what());
        throw;
    }

    Glib::signal_child_watch().connect(sigc::mem_fun(*this, &ProcessExecutor::reap_child), pid);
    logger->info("Spawned sub-process \"" + argv[0] + "\" with PID " + std::to_string(pid) + '.');
}

void ProcessExecutor::reap_child(const Glib::Pid ended_pid, const int exit_code) noexcept
{
    if (ended_pid != pid)
        // Filter PIDs that aren't ours. (Shouldn't ever happen, but isn't worth logging.)
        return;

    Glib::spawn_close_pid(pid);

    if (exit_code == 0)
        logger->info("Subprocess with PID " + std::to_string(pid) + " exited normally.");
    else
        logger->warn("Subprocess with PID " + std::to_string(pid) + " exited with non-zero exit code " +
                std::to_string(exit_code) + '.');

    finished_callback(exit_code);
}

} // namespace optifol
