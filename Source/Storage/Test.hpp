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

#include "StorageObjectBase.hpp"

namespace optifol
{

/**
 * @class Test
 * @brief The Test storage object denotes a single unit test to be executed against a testable target executable.
 */
class Test :
        public StorageObjectBase
{
public:
    explicit Test(const std::string& target_executable);

    Test(const std::string& target_executable, BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>&);

    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_target_executable();

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_target_executable() const;

private:
    Glib::Property<Glib::ustring> target_executable;
};

} // namespace optifol

#endif // TEST_HPP
