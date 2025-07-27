/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for Google Test framework factories.
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#ifndef GOOGLETESTFACTORY_HPP
#define GOOGLETESTFACTORY_HPP

#include <memory>
#include <gtkmm/textbuffer.h>

namespace optifol
{

class ProcessExecutor;

class GoogleTestFactory
{
public:
    static std::unique_ptr<ProcessExecutor> execute_tests(sigc::slot<void(int)> &&finished_callback);

    static std::unique_ptr<ProcessExecutor> dry_run_executable(std::string_view executable_name,
            Glib::RefPtr<Gtk::TextBuffer> output_buffer, sigc::slot<void(int)> &&finished_callback);
};

} // namespace optifol

#endif // GOOGLETESTFACTORY_HPP
