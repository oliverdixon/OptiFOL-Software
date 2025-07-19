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

#include "../IHashable.hpp"

namespace Gtk
{

/*
 * <gtk/builder.h> introduces symbols into the global namespace that clash with Bison-generated code. So we forward-
 * declare it instead, as it is only used as an (unused) parameter l-value reference.
 */
class Builder;

}

namespace optifol
{

class TestResult :
        public IHashable
{
public:
    struct Partial
    {
        Partial() = default;
        Partial(std::string file, std::size_t line, std::string message);

        std::string file;
        std::size_t line = 0;
        std::string message;
    };

    TestResult(std::string test_name, bool passed, std::size_t execution_time,
        std::vector<Partial>&& partial_results = {});

    [[nodiscard]] std::size_t hash() const noexcept override;

    void populate_test_suite_name(std::string suite_name);

    [[nodiscard]] std::string get_test_name() const noexcept;

    [[nodiscard]] bool has_passed() const noexcept;

    [[nodiscard]] std::size_t get_execution_time() const noexcept;

    [[nodiscard]] std::vector<TestResult::Partial> &&steal_partial_results() noexcept;

    [[nodiscard]] std::optional<std::string> get_test_suite_name() const noexcept;

private:
    const std::string test_name;
    const bool passed;
    const std::size_t execution_time;

    std::vector<Partial> partial_results;
    std::optional<std::string> suite_name;
};

} // namespace optifol

#endif // TESTRESULT_HPP
