/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Test-level storage object
 * @author Oliver Dixon
 * @date 2025-07-17
 * @version Development
 */

#ifndef TEST_HPP
#define TEST_HPP

#include "../UserTesting/TestResult.hpp"
#include "StorageObjectBase.hpp"

namespace optifol
{

/**
 * @class Test
 * @brief The Test storage object denotes a single unit test to be executed against a testable target executable.
 */
class Test : public StorageObjectBase
{
public:
    explicit Test(std::string_view packed_input_line);

    Test(const std::string& target_executable, const std::string& test_suite, const std::string& test_name);

    Test(const std::string& target_executable, const std::string& test_suite, const std::string& test_name,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &);

    void emplace_result(const std::shared_ptr<TestResult> &test_result);

    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_target_executable();

    [[nodiscard]] Glib::PropertyProxy<std::optional<Glib::ustring>> property_test_suite();

    [[nodiscard]] Glib::PropertyProxy<std::shared_ptr<TestResult>> property_result();

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_target_executable() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<std::optional<Glib::ustring>> property_test_suite() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<std::shared_ptr<TestResult>> property_result() const;

private:
    Glib::Property<Glib::ustring> target_executable;
    Glib::Property<std::optional<Glib::ustring>> test_suite;
    Glib::Property<std::shared_ptr<TestResult>> result;
};

} // namespace optifol

#endif // TEST_HPP
