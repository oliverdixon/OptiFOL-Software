/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/16/25.
//

#ifndef TESTRESULT_HPP
#define TESTRESULT_HPP

#include <cstddef>
#include <glibmm/object.h>
#include <glibmm/property.h>
#include <string>
#include <vector>

namespace Gtk
{

/*
 * <gtk/builder.h> introduces symbols into the global namespace that clash with Bison-generated code. So we forward-
 * declare it instead, as it's only used as an (unused) parameter l-value reference.
 */
class Builder;

}

namespace optifol
{

class TestResult :
        public Glib::Object
{
public:
    struct Partial
    {
        Partial() = default;
        Partial(const std::string& file, std::size_t line, const std::string& message);

        std::string file;
        std::size_t line;
        std::string message;
    };

    TestResult(bool passed, std::size_t execution_time, std::vector<Partial>&& partial_results = {});

    TestResult(bool passed, std::size_t execution_time, std::vector<Partial>&& partial_results,
        BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

    [[nodiscard]] Glib::PropertyProxy<bool> property_passed();

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<bool> property_passed() const;

    [[nodiscard]] Glib::PropertyProxy<std::size_t> property_execution_time();

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<std::size_t> property_execution_time() const;

    [[nodiscard]] Glib::PropertyProxy<std::vector<Partial>> property_partial_results();

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<std::vector<Partial>> property_partial_results() const;

private:
    Glib::Property<bool> passed;

    Glib::Property<std::size_t> execution_time;

    Glib::Property<std::vector<Partial>> partial_results;
};

} // namespace optifol

#endif // TESTRESULT_HPP
