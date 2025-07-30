/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/29/25.
//

#ifndef GOOGLETESTEXECUTABLE_HPP
#define GOOGLETESTEXECUTABLE_HPP

#include "../../GUI/ProcessExecutor.hpp"
#include "../TargetTestExecutableBase.hpp"

namespace optifol
{

class GoogleTestExecutable : public TargetTestExecutableBase
{
public:
    explicit GoogleTestExecutable(const Glib::ustring &executable_path);

    GoogleTestExecutable(const Glib::ustring &executable_path, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder);

private:
    void start_discovery();

    void discovery_done_callback(int exit_code);

    std::unique_ptr<ProcessExecutor> discovery_executor;
    std::optional<std::string> discovery_tmp_file_path;
};

} // namespace optifol

#endif // GOOGLETESTEXECUTABLE_HPP
