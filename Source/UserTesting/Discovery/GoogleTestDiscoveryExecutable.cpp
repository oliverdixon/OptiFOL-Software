/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the GoogleTestDiscoveryExecutable discovery executable backend
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#include "GoogleTestDiscoveryExecutable.hpp"

#include <cassert>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

namespace optifol
{

GoogleTestDiscoveryExecutable::GoogleTestDiscoveryExecutable(const Glib::ustring &executable_path) :
    Glib::ObjectBase("GoogleTestDiscoveryExecutable"),
    DiscoveryTestExecutable(executable_path)
{
    start_discovery();
}

GoogleTestDiscoveryExecutable::GoogleTestDiscoveryExecutable(
        const Glib::ustring &executable_path, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("GoogleTestDiscoveryExecutable"),
    DiscoveryTestExecutable(executable_path, cobject, builder)
{
    start_discovery();
}

void GoogleTestDiscoveryExecutable::start_discovery()
{
    const auto current_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    discovery_tmp_file_path.emplace("/tmp/google_test_discovery." + std::to_string(current_time_ms.count()) + ".json");
    discovery_executor = std::make_unique<ProcessExecutor>(
        "", // Current working directory.
        std::vector<std::string>{
            property_name().get_value(),
            "--gtest_list_tests",
            "--gtest_output=json:" + *discovery_tmp_file_path
        },
        std::vector<std::string>{},
        sigc::mem_fun(*this, &GoogleTestDiscoveryExecutable::discovery_done_callback)
    );
}

void GoogleTestDiscoveryExecutable::discovery_done_callback(const int exit_code)
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

    // TODO URGENT make this all more robust. Don't do asserts. Throw exceptions.
    for (const auto& fixture : fixtures.GetArray()) {
        assert(fixture.IsObject());
        const auto& fixture_name = fixture["name"];
        assert(fixture_name.IsString());
        const auto& fixture_tests = fixture["testsuite"];
        assert(fixture_tests.IsArray());

        const auto modelled_fixture = Glib::make_refptr_for_instance(
            new DiscoveryTestFixture(fixture_name.GetString()));

        for (const auto& test : fixture_tests.GetArray()) {
            assert(test.IsObject());
            const auto& test_name = test["name"];
            assert(test_name.IsString());
            modelled_fixture->add_test(test_name.GetString());
        }

        add_fixture(modelled_fixture);
    }

    std::remove(discovery_tmp_file_path->c_str());
    discovery_tmp_file_path.reset();
}

} // namespace optifol
