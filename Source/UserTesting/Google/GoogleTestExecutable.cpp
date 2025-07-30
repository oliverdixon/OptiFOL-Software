/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/29/25.
//

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <fstream>
#include <iostream>

#include "GoogleTestExecutable.hpp"

#include "../../GUI/ProcessExecutor.hpp"

namespace optifol
{

GoogleTestExecutable::GoogleTestExecutable(const Glib::ustring &executable_path) :
    Glib::ObjectBase("GoogleTestExecutable")
{
    property_name().set_value(executable_path);
    start_discovery();
}

GoogleTestExecutable::GoogleTestExecutable(
        const Glib::ustring &executable_path, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("GoogleTestExecutable"),
    TargetTestExecutableBase(cobject, builder)
{
    property_name().set_value(executable_path);
    start_discovery();
}

void GoogleTestExecutable::start_discovery()
{
    const auto current_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    discovery_tmp_file_path.emplace("/tmp/google_test_discovery." + std::to_string(current_time_ms.count()) + ".json");
    discovery_executor = std::make_unique<ProcessExecutor>(
        "",
        std::vector<std::string>{
            property_name().get_value(),
            "--gtest_list_tests",
            "--gtest_output=json:" + *discovery_tmp_file_path
        },
        std::vector<std::string>{},
        sigc::mem_fun(*this, &GoogleTestExecutable::discovery_done_callback)
    );
}

void GoogleTestExecutable::discovery_done_callback(const int exit_code)
{
    std::ignore = exit_code;
    assert(discovery_tmp_file_path.has_value());

    discovery_executor.reset();
    std::ifstream discovery_file_stream{*discovery_tmp_file_path};
    // TODO document if failed to open.

    rapidjson::IStreamWrapper json_stream{discovery_file_stream};
    rapidjson::Document document;
    document.ParseStream(json_stream);

    const auto& fixtures = document["testsuites"];
    assert(fixtures.IsArray());

    for (const auto& fixture : fixtures.GetArray()) {
        assert(fixture.IsObject());
        const auto& name = fixture["name"];
        assert(name.IsString());
        fixtures_model->append(name.GetString());
    }

    std::remove(discovery_tmp_file_path->c_str());
    discovery_tmp_file_path.reset();
}

} // namespace optifol
