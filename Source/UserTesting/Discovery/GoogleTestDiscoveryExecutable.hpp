/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/31/25.
//

#ifndef GOOGLETESTDISCOVERYEXECUTABLE_HPP
#define GOOGLETESTDISCOVERYEXECUTABLE_HPP

#include "../../GUI/ProcessExecutor.hpp"
#include "DiscoveryTestExecutable.hpp"

namespace optifol
{

class GoogleTestDiscoveryExecutable : public DiscoveryTestExecutable
{
public:
    explicit GoogleTestDiscoveryExecutable(const Glib::ustring& name);

    GoogleTestDiscoveryExecutable(const Glib::ustring& name, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder);

private:
    void start_discovery();

    void discovery_done_callback(int exit_code);

    std::unique_ptr<ProcessExecutor> discovery_executor;
    std::optional<std::string> discovery_tmp_file_path;
};

} // namespace optifol

#endif // GOOGLETESTDISCOVERYEXECUTABLE_HPP
