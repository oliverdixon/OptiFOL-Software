/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the TestResult results storage
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#ifndef TESTRESULT_HPP
#define TESTRESULT_HPP

#include <glibmm/ustring.h>
#include <optional>
#include <string>
#include <vector>

#include "../../IHashable.hpp"

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

    void populate_test_fixture_name(const std::string &incoming_fixture_name);

    [[nodiscard]] Glib::ustring get_test_name() const noexcept;

    [[nodiscard]] bool has_passed() const noexcept;

    [[nodiscard]] std::size_t get_execution_time() const noexcept;

    [[nodiscard]] Glib::ustring get_fixture_name() const noexcept;

    bool operator==(const TestResult & other) const noexcept;

    bool operator==(const std::pair<Glib::ustring, Glib::ustring>& names) const noexcept;

private:
    const Glib::ustring test_name;
    Glib::ustring fixture_name; // Cannot be const, as parsers may load the fixture metadata after its results.

    const bool passed;
    const std::size_t execution_time;
    std::vector<Partial> partial_results;
};

} // namespace optifol

template<>
struct std::hash<optifol::TestResult>
{
    using is_transparent = void;

    std::size_t operator()(const optifol::TestResult * object) const noexcept
    {
        return object->hash();
    }

    std::size_t operator()(const std::pair<Glib::ustring, Glib::ustring>& names) const noexcept
    {
        return optifol::IHashable::hash_combine(std::hash<std::string>{}(names.first),
            std::hash<std::string>{}(names.second));
    }

    std::size_t operator()(const optifol::TestResult & object) const noexcept
    {
        return object.hash();
    }

    std::size_t operator()(const std::shared_ptr<optifol::TestResult> &shared_hashable) const noexcept
    {
        return shared_hashable->hash();
    }
};

#endif // TESTRESULT_HPP
