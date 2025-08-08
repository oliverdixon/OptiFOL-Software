/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the framework-agnostic execution group
 * @author Oliver Dixon
 * @date 2025-08-08
 * @version Development
 */

#ifndef EXECUTIONGROUP_HPP
#define EXECUTIONGROUP_HPP

#include <glibmm/refptr.h>
#include <glibmm/ustring.h>
#include <sigc++/functors/slot.h>
#include <unordered_set>

#include "../../HashableEqualityFunctor.hpp"
#include "../../IHashable.hpp"

namespace optifol
{

class TestExecutable;
class Test;

class ExecutionGroup : public IHashable
{
public:
    [[nodiscard]] std::size_t hash() const noexcept override;

    virtual void add_test(Glib::RefPtr<Test> new_test) = 0;

    virtual void remove_test(const Glib::RefPtr<Test> &target_test) = 0;

    virtual void run() = 0;

    [[nodiscard]] virtual std::size_t is_empty() const noexcept = 0;

    [[nodiscard]] bool operator==(const TestExecutable& other) const;

    [[nodiscard]] bool operator==(const ExecutionGroup& other) const;

    [[nodiscard]] Glib::ustring get_executable_name() const;

protected:
    explicit ExecutionGroup(const TestExecutable& executable);

    [[nodiscard]] bool check_eligibility(const Test& test) const;

private:
    const TestExecutable& executable;
};

struct ExecutionGroupEqualityFunctor : HashableEqualityFunctor<ExecutionGroup>
{
    bool operator()(const std::unique_ptr<ExecutionGroup> &lhs, const TestExecutable& rhs) const;

    bool operator()(const TestExecutable& lhs, const std::unique_ptr<ExecutionGroup> &rhs) const;

    bool operator()(const std::unique_ptr<ExecutionGroup> &lhs, const std::unique_ptr<ExecutionGroup> &rhs) const;
};

} // namespace optifol

template<>
struct std::hash<optifol::ExecutionGroup>
{
    using is_transparent = void;

    std::size_t operator()(const optifol::ExecutionGroup& object) const noexcept;

    std::size_t operator()(const optifol::TestExecutable& executable) const noexcept;

    std::size_t operator()(const unique_ptr<optifol::ExecutionGroup> &unique_hashable) const noexcept;
};


#endif // EXECUTIONGROUP_HPP
