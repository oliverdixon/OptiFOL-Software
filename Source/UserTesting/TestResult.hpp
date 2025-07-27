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

#include <optional>
#include <string>
#include <vector>

#include "../IHashable.hpp"

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

    [[nodiscard]] std::optional<std::string> get_test_suite_name() const noexcept;

    bool operator==(const TestResult & other) const noexcept;

    bool operator==(const std::pair<std::string_view, std::string_view>& names) const noexcept;

private:
    const std::string test_name;
    const bool passed;
    const std::size_t execution_time;

    std::vector<Partial> partial_results;
    std::optional<std::string> suite_name;
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

    std::size_t operator()(const std::pair<std::string_view, std::string_view>& names) const noexcept
    {
        return optifol::IHashable::hash_combine(std::hash<std::string_view>{}(names.first),
            std::hash<std::string_view>{}(names.second));
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
