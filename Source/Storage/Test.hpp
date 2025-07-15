/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/15/25.
//

#ifndef TEST_HPP
#define TEST_HPP

#include "StorageObjectBase.hpp"

namespace optifol
{

class Test :
        public StorageObjectBase
{
public:
    enum class Result
    {
        Pass,
        Fail
    };

    explicit Test();

    explicit Test(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_target_executable();

    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_test_suite();

    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_test_name();

    [[nodiscard]] Glib::PropertyProxy<std::size_t> property_execution_time_ms();

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_target_executable() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_test_suite() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_test_name() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<std::size_t> property_execution_time_ms() const;

private:
    Glib::Property<Glib::ustring> target_executable;
    Glib::Property<Glib::ustring> test_suite;
    Glib::Property<Glib::ustring> test_name;
    Glib::Property<std::size_t> execution_time_ms;
};

} // namespace optifol

#endif // TEST_HPP
